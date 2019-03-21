#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

// Variables globales
int acum_lecturas = 0;
static struct proc_dir_entry *proc_entry;
unsigned long prev_random;
static char *input_semilla;

//Funcion generadora de numeros aleatorios.
unsigned long get_random(unsigned long m_w, unsigned long m_z)
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 1800 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;
}

//Funciones de lectura y escritura invocadas por /proc fs.
int cant_lecturas(char * page, char **start, off_t off, int count, int *eof, void *data)
{
    int len;
    len = sprintf(page, "Lecturas realizadas: %d\n", acum_lecturas);
    return len;
}

ssize_t cambiar_semilla(struct file *filp, const char __user *buff, unsigned long len, void *data)
{
    copy_from_user(&input_semilla[0], buff, len);
    prev_random = simple_strtoul(input_semilla, NULL, 10);
    printk(KERN_INFO "probabilidad: Se ha cambiado la semilla\n");
    return len;
}

//Funciones de lectura invocada por /dev fs
static ssize_t probabilidad_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    unsigned int random_pos;
    char *probabilidad_str;
    char random_letter;
    int len;

    prev_random = get_random(prev_random, prev_random);
    random_pos = (prev_random % 25);
    probabilidad_str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    random_letter = probabilidad_str[random_pos];
    len=1;

    if (count < len) return -EINVAL;
    if (*ppos !=0 ) return 0;
    if (copy_to_user(buf, &random_letter,len))
        return -EINVAL;
    *ppos = len;
    acum_lecturas++;
    printk(KERN_ALERT "probabilidad: Se ha utilizado la semilla %lu\n", simple_strtoul(input_semilla, NULL, 10));
    return len;
}

// Estructuras utililizadas por la funcion de registro de dispositivos
static const struct file_operations probabilidad_fops = {
    .owner = THIS_MODULE,
    .read = probabilidad_read
};

static struct miscdevice probabilidad_dev = {
    MISC_DYNAMIC_MINOR,
    "probabilidad",
    &probabilidad_fops
};

// Funciones utilizadas por la creacion y destruccion del modulo
static int __init probabilidad_init(void) {
    int ret, prec_random;
    // Registración del device
    ret = misc_register(&probabilidad_dev);
    if (ret)
    {
        printk(KERN_ERR "No se puede registrar el dispositivo PROBABILIDAD\n");
    } else
    {
        // Inicializacion de la semilla del random
        struct timespec tv;
        getnstimeofday(&tv);
        prec_random=tv.tv_nsec;
        input_semilla= (char *) vmalloc(PAGE_SIZE);
        memset(input_semilla, 0, PAGE_SIZE);

        // Definicion de la entrada en /proc con la asociacion de funciones de lectura y escritura
        proc_entry = create_proc_entry("probabilidad", 0644, NULL);
        if (proc_entry == NULL)
        {
            printk(KERN_INFO "probabilidad: No se pudo crear la entrada en /proc\n");
        } else
        {
            proc_entry->read_proc=cant_lecturas;
            proc_entry->write_proc=cambiar_semilla;
        }
    }
    return ret;
}

static void __exit probabilidad_exit(void) {
    remove_proc_entry("probabilidad", proc_entry);
    vfree(input_semilla);
    misc_deregister(&probabilidad_dev);
}

// Definicion de constructor y destructor del modulo.
module_init(probabilidad_init);
module_exit(probabilidad_exit);

// Todo esto se usa con "echo SEED > /proc/probabilidad"  y "cat /proc/probabilidad"
// para poner la nueva semilla y para ver la cantidad de lecturas realizadas.
// Y con "cat /dev/probabilidad" para ver la letra aleatoria generada.


