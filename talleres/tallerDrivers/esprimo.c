#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

int numero_actual;

bool esPrimo(int n){
	// El 1 no es primo
	if(n==1) return false;
	
	int d = 2;
	while(d*d <= n){
		if(n%d == 0) return false;
	}
	return true;
}

// Variables globales
int acum_lecturas = 0;
static char input_primos[1024];

//Funciones de lectura y escritura invocadas por /proc fs.
int cant_lecturas(char * page, char **start, off_t off, int count, int *eof, void *data)
{
    int len;
    len = sprintf(page, "Lecturas/Escrituras realizadas: %d\n", acum_lecturas);
    return len;
}

static ssize_t esprimo_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    int cant=0;

    if (copy_from_user(input_primos, buf, len)) return -EFAULT;

    cant=sscanf(input_primos, "%d", &numero_actual);
    if (cant!=1) {
        printk(KERN_INFO "esprimo: Error al cambiar valores iniciales\n");
        return -EINVAL;
    }
    else
        printk(KERN_INFO "esprimo: Cambio de variables a %d\n", numero_actual);

    return len;
}

//Funciones de lectura invocada por /dev fs
static ssize_t esprimo_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    int len;
    char buffer[20];

    if (count < 1) return -EINVAL;
    if (*ppos !=0 ) return 0;

    printk(KERN_INFO "Imprimiendo resultado para %d\n", numero_actual);
    len = sprintf(buffer, "%d\n", esPrimo(numero_actual));

    if (copy_to_user(buf, buffer,len))
        return -EINVAL;
    *ppos = len;
    acum_lecturas++;
    return len;
}

// Estructura del misc device
static struct file_operations esprimo_fops =
{
        .owner   = THIS_MODULE,
        .read    = esprimo_read,
        .write   = esprimo_write
};

static struct miscdevice esprimo_dev = {
	MISC_DYNAMIC_MINOR,
	"esprimo",
	&esprimo_fops
};

// Funciones utilizadas por la creacion y destruccion del modulo
static int __init esprimo_init(void) {
    int ret;
    // Registración del device
    ret = misc_register(&esprimo_dev);
    if (ret)
    {
        printk(KERN_ERR "No se puede registrar el dispositivo ESPRIMO\n");
    } else
    {
        numero_actual = 0;
    }
    return ret;
}

static void __exit esprimo_exit(void) {
    misc_deregister(&esprimo_dev);
}

// Definicion de constructor y destructor del modulo.
module_init(esprimo_init);
module_exit(esprimo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("aventura clang");
MODULE_DESCRIPTION("Comprobador de primalidad de numeros enteros.");
