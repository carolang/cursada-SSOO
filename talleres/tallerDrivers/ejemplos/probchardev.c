#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kd.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/ioctl.h>
#include <linux/syscalls.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/tty.h>
#include <linux/vt_kern.h>
#include <linux/console_struct.h>
#include <linux/random.h>
#include <linux/kdev_t.h>
#include <asm/uaccess.h>

/* Prototipos de las funciones de inicializacion y destruccion */
static int __init proba_init(void);
static void __exit proba_exit(void);

// Prototipos de las funciones de manejo del dispositivo
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);

module_init(proba_init);
module_exit(proba_exit);

#define SUCCESS 0
#define PROC_FILE_SIZE sizeof(int) // Un entero
#define SEMILLA_DEFAULT 1238732
#define procfs_name "probabilidad" // Nombre del archivo en /proc
#define DEVICE_NAME "probabilidad"

static struct proc_dir_entry *procFile; // Informacion de nuestro archivo en /proc
static int cantidadLecturas;
static int Major; // Device major number
// Funciones de manejo del dispositivo en /dev
static struct file_operations fops =
{
        .read    = device_read,
        .open    = device_open,
        .release = device_release
};

// Maneja aperturas del archivo
static int device_open(struct inode *inode, struct file *file)
{
        try_module_get(THIS_MODULE);
        return SUCCESS;
}

// Maneja el evento de cierre del archivo
static int device_release(struct inode *inode, struct file *file)
{
        module_put(THIS_MODULE);
        return SUCCESS;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length,
                                loff_t *offset)
{
        size_t i;
        cantidadLecturas++;
        for(i = 0; i < length; i++)
        {
                unsigned int r = random32();
                buffer[i] = 'A' + r%26;
        }
        return length;
}

// Funciones de manejo de la entrada en /proc
// Devuelve al usuario la cantidad de lecturas realizadas.
int procFileRead(char *buffer,char **buffer_location,off_t 
                offset,  int bufferLength, int *eof, void *data)
{
        if (offset>=PROC_FILE_SIZE || offset < 0) return 0;
        else 
        {
                sprintf(buffer,"%d",cantidadLecturas);
//                *buffer_location = (char *)(&cantidadLecturas) + offset;
                return PROC_FILE_SIZE-offset;
        }
}

// Lee la semilla del usuario y la utiliza.
int procFileWrite(struct file *file, const char *buffer, 
                unsigned long count, void *data)
{
        u32 semilla = 0;
        // No se modifica nada si el offset esta fuera de rango.
        if (file->f_pos < 0 || file->f_pos >= PROC_FILE_SIZE) return count; 
        copy_from_user(&semilla,buffer,min((unsigned long)sizeof(semilla),count));
        srandom32(semilla);
        printk(KERN_ALERT "Se cambia la semilla por %u\n",semilla);
        return count;
}

/* Inicializacion */
static int __init proba_init()
{
        printk(KERN_ALERT "Se carga el modulo probabilidad\n");
        cantidadLecturas = 0;
        srandom32(SEMILLA_DEFAULT);

        // Inicializamos el dispositivo en /dev
        Major = register_chrdev(0,DEVICE_NAME,&fops);
        if (Major < 0)
        {
                printk(KERN_ALERT "Registering char device failed with %d\n",Major);
                return Major;
        }
        printk(KERN_ALERT "Se registro el driver con Major number = %d\n",Major);

        // Inicializamos el archivo en /proc
        procFile = create_proc_entry(procfs_name, 0666,NULL);
        if (procFile == NULL)
        {
                remove_proc_entry(procfs_name, procFile);
                printk(KERN_ALERT "Error: Could not initialize /proc/%s\n",procfs_name);
                unregister_chrdev(Major, DEVICE_NAME);
                return -ENOMEM;
        }

        procFile->read_proc  = procFileRead;
        procFile->write_proc = procFileWrite;
        //procFile->owner      = THIS_MODULE;
        procFile->mode       = S_IFREG | S_IRUGO | S_IWUGO; // rw-rw-rw-
        procFile->uid        = 0;
        procFile->gid        = 0;
        procFile->size       = PROC_FILE_SIZE;

        /* Si devolvemos un valor distinto de cero significa que
         * hello_init fallo y el modulo no puede ser cargado.
         */
        return SUCCESS;
}

/* Destruccion */
static void __exit proba_exit()
{
        printk(KERN_ALERT "Se descarga el modulo probabilidad\n");
        remove_proc_entry(procfs_name,procFile);
        unregister_chrdev(Major,DEVICE_NAME);
}

MODULE_LICENSE("GPL");
