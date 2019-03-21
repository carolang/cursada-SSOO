#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>
#include <linux/vt_kern.h>
//#include <linux/types.h>
//#include <linux/proc_fs.h>
//#include <linux/fs.h>
//#include <linux/kdev_t.h>
#include <asm/agp.h>
//#include <linux/init.h>
//#include <linux/kd.h>
//#include <linux/fcntl.h>
//#include <linux/ioctl.h>
//#include <linux/syscalls.h>
//#include <linux/tty.h>
//#include <linux/console_struct.h>
//#include <linux/random.h>

/* Prototipos de las funciones de inicializacion y destruccion */
static int __init sysmk_init(void);
static void __exit sysmk_exit(void);

/* Informamos al kernel que inicialice el modulo usando hello_init
* y que antes de quitarlo use hello_exit
*/

module_init(sysmk_init);
module_exit(sysmk_exit);

#define SUCCESS 0
// La direccion de la tabla se puede obtener: grep sys_call_table /boot/System.map*
// #define SYS_CALL_TABLE_BASE 0xc0318500
#define SYS_CALL_TABLE_BASE 0x806a88e0

static const void **sys_call_table = (void *) SYS_CALL_TABLE_BASE;

asmlinkage int (*sys_call_orig)(const char *pathname);
static struct page *pg;
//static pgprot_t prot;

int miMkdir(const char *path) {        return -EPERM; } // Siempre falla

/* Inicializacion */
static int __init sysmk_init()
{
        printk(KERN_ALERT "Se carga el modulo nomkdir\n");

        sys_call_orig = sys_call_table[__NR_mkdir];

        pg = virt_to_page(SYS_CALL_TABLE_BASE);
//        prot.pgprot = VM_READ | VM_WRITE | VM_EXEC;
//        change_page_attr(pg,1,prot);

        set_pages_rw(pg,1);
        sys_call_table[__NR_mkdir] = miMkdir;

        return SUCCESS;
}

/* Destruccion */
static void __exit sysmk_exit()
{
        printk(KERN_ALERT "Se descarga el modulo nomkdir\n");
        
        sys_call_table[__NR_mkdir] = sys_call_orig;
}

MODULE_LICENSE("GPL");

