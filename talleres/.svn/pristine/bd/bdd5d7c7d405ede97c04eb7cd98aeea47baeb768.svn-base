#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

// Variables globales
int acum_lecturas = 0;
static struct proc_dir_entry *proc_entry;
static unsigned long prev_fib, prev_fib2;
static char *input_fibos;

//Funcion generadora de numeros fibonacci.
unsigned long get_fibona(unsigned long m_w, unsigned long m_z)
{
    return (m_z + m_w);
}

//Funciones de lectura y escritura invocadas por /proc fs.
int cant_lecturas(char * page, char **start, off_t off, int count, int *eof, void *data)
{
    int len;
    len = sprintf(page, "Lecturas/Escrituras realizadas: %d\n", acum_lecturas);
    return len;
}

static ssize_t fibonacci_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    int cant=0;

    if ((len < 3)||(len>=PAGE_SIZE)) return -EINVAL;
    cant = copy_from_user(&input_fibos[0], buf, len);
    if (cant != 0)
          return -EFAULT;
    cant=sscanf(input_fibos, "%lu %lu", &prev_fib, &prev_fib2);
    if (cant!=2) {
        printk(KERN_INFO "fibo: Error al cambiar valores iniciales\n");
        return -EINVAL;
    }
    else
        printk(KERN_INFO "fibo: Cambio de variables a %lu %lu\n", prev_fib, prev_fib2);

    *ppos = len;
    acum_lecturas++;
    return len;
}

//Funciones de lectura invocada por /dev fs
static ssize_t fibonacci_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    int len;
    unsigned long fibo;
    char buffer[20];

    if (count < 1) return -EINVAL;
    if (*ppos !=0 ) return 0;

    printk(KERN_INFO "fibo: variables %d %lu %lu\n", (int) count, prev_fib, prev_fib2);
    fibo = get_fibona(prev_fib, prev_fib2);
    len = sprintf(buffer, "%lu", fibo);
    prev_fib=prev_fib2;
    prev_fib2=fibo;

    if (copy_to_user(buf, buffer,len))
        return -EINVAL;
    *ppos = len;
    acum_lecturas++;
    return len;
}

// Estructuras utililizadas por la funcion de registro de dispositivos
static const struct file_operations fibonacci_fops = {
    .owner = THIS_MODULE,
    .read = fibonacci_read,
    .write = fibonacci_write
};

static struct miscdevice fibonacci_dev = {
    MISC_DYNAMIC_MINOR,
    "fibonacci",
    &fibonacci_fops
};

// Funciones utilizadas por la creacion y destruccion del modulo
static int __init fibonacci_init(void) {
    int ret;
    // Registración del device
    ret = misc_register(&fibonacci_dev);
    if (ret)
    {
        printk(KERN_ERR "No se puede registrar el dispositivo FIBONACCI\n");
    } else
    {
        // Inicializacion de la semilla del random
        prev_fib=0;
        prev_fib2=1;
        input_fibos= (char *) vmalloc(PAGE_SIZE);
        memset(input_fibos, 0, PAGE_SIZE);

        // Definicion de la entrada en /proc con la asociacion de funciones de lectura y escritura
        proc_entry = create_proc_entry("fibocant", 0644, NULL);
        if (proc_entry == NULL)
        {
            printk(KERN_INFO "fibonacci: No se pudo crear la entrada en /proc\n");
        } else
        {
            proc_entry->read_proc=cant_lecturas;
        }
    }
    return ret;
}

static void __exit fibonacci_exit(void) {
    remove_proc_entry("fibocant", NULL);
    vfree(input_fibos);
    misc_deregister(&fibonacci_dev);
}

// Definicion de constructor y destructor del modulo.
module_init(fibonacci_init);
module_exit(fibonacci_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ARG SA");
MODULE_DESCRIPTION("generador de sucesion fibonacci A_i "
    "(con misc device indicando A_0 y A_1)");
