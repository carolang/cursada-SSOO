#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kd.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/ioctl.h>
#include <linux/syscalls.h>
#include <linux/proc_fs.h>
#include <linux/tty.h>
#include <linux/vt_kern.h>
#include <linux/console_struct.h>
#include <asm/uaccess.h>

/* Prototipos de las funciones de inicializacion y destruccion */
static int __init luces_init(void);
static void __exit luces_exit(void);

// Prototipos de funciones de leds
static void setLeds(long int);
static long int getLeds(void);
static void restaurarLeds(void);

/* Informamos al kernel que inicialice el modulo usando luces_init
* y que antes de quitarlo use luces_exit
*/

module_init(luces_init);
module_exit(luces_exit);


// Definicion de las variables globales

#define FILE_SIZE 4 // Un caracter por cada led, y un end of line al final.
#define procfs_name "keyboardLeds" // Nombre del archivo en /proc

long int originalLeds;
char miBuffer[4];
const long int LED_FLAGS[4] = {LED_NUM,LED_CAP,LED_SCR};
const char LED_CHARACTERS[4] = {'n','c','s'};

struct tty_driver *mi_driver; // Driver de la consola
struct proc_dir_entry *procFile; // Informacion de nuestro archivo en /proc


/* Inicializacion */
static int __init luces_init()
{
        printk(KERN_ALERT "Se carga el modulo de control de LEDs\n");

        // Inicializamos lo relativo al driver de la consola.
        mi_driver = vc_cons[fg_console].d->vc_tty->driver;

        // Al momento de cargar el modulo, mostramos el estado de los LEDs
        mostrarLEDs(originalLeds = getLeds());

        // Inicializamos el archivo en /proc
        procFile = create_proc_entry(procfs_name, 0666,&proc_root);
        if (procFile == NULL)
        {
                remove_proc_entry(procfs_name, &proc_root);
                printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",procfs_name);
                return -ENOMEM;
        }

        // Asociamos las funciones de lectura y escritura al archivo creado
        procFile->read_proc  = procFileRead;
        procFile->write_proc = procFileWrite;
        procFile->owner      = THIS_MODULE;
        procFile->mode       = S_IFREG | S_IRUGO | S_IWUGO; // rw-rw-rw-
        procFile->uid        = 0;
        procFile->gid        = 0;
        procFile->size       = FILE_SIZE;
        return 0;
}

/* Destruccion */
static void __exit luces_exit()
{
        printk(KERN_ALERT "Se descarga el modulo de control de LEDs.\n");
        restaurarLeds();
        remove_proc_entry(procfs_name,&proc_root);
}


/* Declaramos que este codigo tiene licencia GPL.
*/
MODULE_LICENSE("GPL");

static void mostrarLEDs(long int leds)
{
        if (leds & LED_NUM) {printk(KERN_ALERT "Num Lock LED is ON\n");}
        if (leds & LED_CAP) {printk(KERN_ALERT "Caps Lock LED is ON\n");}
        if (leds & LED_SCR) {printk(KERN_ALERT "Scroll Lock LED is ON\n");}
}

static long int getLeds(void)
{
        mm_segment_t old_fs;
        long int res;
        
        // Tenemos que cambiar de segmento de usuario a segmento de kernel,
        // porque ioctl esta preparado para ser invocado por el usuario,
        // y por lo tanto interpreta los punteros como punteros en el
        // espacio de direcciones del usuario, mientras que nosotros lo
        // invocamos desde el kernel.
        old_fs = get_fs();
        set_fs(KERNEL_DS);
        mi_driver->ioctl(vc_cons[fg_console].d->vc_tty,NULL,KDGETLED,(int)&res);
        set_fs(old_fs);
        return res;
}

static void setLeds(long int leds)
{ mi_driver->ioctl(vc_cons[fg_console].d->vc_tty,NULL,KDSETLED,leds); }

static void restaurarLeds(void) { setLeds(originalLeds); }

void updateBuffer(void)
{
        int i;
        int leds = getLeds();
        for(i=0;i<3;i++) 
                miBuffer[i] = ((leds&LED_FLAGS[i])?LED_CHARACTERS[i]:'-');
        miBuffer[3] = '\n';
}

int procFileRead(char *buffer,char **buffer_location,off_t offset, 
                                int bufferLength, int *eof, void *data)
{
        if (offset>=FILE_SIZE || offset < 0) return 0;
        else 
        {
                updateBuffer();
                *buffer_location = miBuffer+offset;
                return FILE_SIZE-offset;
        }
}

int procFileWrite(struct file *file, const char *buffer, unsigned long count, void *data)
{
        long int leds;
        unsigned long oCount;
        typeof(file->f_pos) i;
        char writeBuffer[4];

        if (file->f_pos < 0) return count; // No se modifica nada si el offset es negativo.
        oCount = count;
        leds = getLeds() & (LED_NUM | LED_CAP | LED_SCR);

        copy_from_user(writeBuffer,buffer,min(3UL,count));
        if (count > 0 && writeBuffer[0] == 'X') {
                // Si se escribe cualquier cosa que tenga a 'X' como
                // primer caracter, se interpreta que se debe restaurar
                // los leds al estado original al cargar el modulo.
                restaurarLeds();
                return oCount;
        }
        for (i=file->f_pos; i < 3 && count > 0; i++,count--) {
                if (writeBuffer[oCount-count] == LED_CHARACTERS[i])
                        leds |= LED_FLAGS[i];    // Se Prende el led
                else if (writeBuffer[oCount-count] == '-')
                        leds &= ~(LED_FLAGS[i]); // Se Apaga el led
        }
        setLeds(leds);
        return oCount;
}

