#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kd.h>
#include <linux/proc_fs.h>
#include <linux/tty.h>
#include <linux/vt_kern.h>
#include <linux/console_struct.h>
#include <asm/uaccess.h>

extern int fg_console;

struct tty_driver *my_driver;
int kbledstatus = 0;

#define RESTORE_LEDS     0xFF
#define PROCFS_NAME      "luces"

/* 
* Estructura para el archivo en /proc 
*/

static struct proc_dir_entry *Our_Proc_File;

/* 
* Funcion que se llama cuando se lee del / proc
* No hacemos nada
*/
int procfile_read( char *buffer, char **buffer_location,
          off_t offset, int buffer_length, int *eof, void *data)
{
   char Estado[4]="___\0";
   int len;
   if (kbledstatus & 0x01) Estado[2]='S';
   if (kbledstatus & 0x02) Estado[1]='N';
   if (kbledstatus & 0x04) Estado[0]='C';
          
   len = sprintf(buffer, "Estado de Luces: %s\n", Estado);
//   printk(KERN_INFO "kbproc: DUMMY procfile_read (/proc/%s)called \n", PROCFS_NAME ) ;
   return len;
}

/* 
* Funcion que se llama cuando se graba el / proc 
*/
int procfile_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
   int i =0;
   printk(KERN_INFO "kbproc: procfile_write (/proc/%s) called\n" , PROCFS_NAME);
   while ((i<count) && (i < 5))
   {
     char cKey = buffer[i];
     if ( cKey == 'x' )
     {
         /* ALL off */
         printk(KERN_INFO "kbproc: ALL ON\n");
         kbledstatus = 0x0;
     }
     if ( cKey == 'X' )
     {
         /* ALL on */
         printk(KERN_INFO "kbproc: ALL ON\n");
         kbledstatus = 0x7;
     }
     if ( cKey == 'n' )
     {
         /* Numlock off */
         printk(KERN_INFO "kbproc: NumLock off\n");
         kbledstatus = kbledstatus & 0xFD ;
      }
      if ( cKey == 'N' )
      {
         /* Numlock on */
         printk(KERN_INFO "kbproc: NumLock on\n");
         kbledstatus = kbledstatus | 0x02 ;
      }
      if ( cKey == 'c' )
      {
         /* Capslock off */
         printk(KERN_INFO "kbproc: CapsLock off\n");
         kbledstatus = kbledstatus & 0xFB ;
      }
      if ( cKey == 'C' )
      {
         /* Capslock on */
         printk(KERN_INFO "kbproc: CapsLock on\n");
         kbledstatus = kbledstatus | 0x04 ;
      }
      if ( cKey == 's' )
      {
         /* Scrollock off */
         printk(KERN_INFO "kbproc: ScrolLock off\n");
         kbledstatus = kbledstatus & 0xFE ;
      }
      if ( cKey == 'S')
      {
         /* Scrollock on */
         printk(KERN_INFO "kbproc: ScrolLock on\n");
         kbledstatus = kbledstatus | 0x01 ;
      }
      i++;
   }

   /*
     * Seteamos en valor de las luces del teclado en la tty actual
     * ( fg_console ) con el valor kbledstatus
     */
   (my_driver->ops->ioctl)(vc_cons[fg_console].d->vc_tty, NULL, KDSETLED, kbledstatus);
   return count ;
}

 /* Funcion de inicializacion del modulo */
 static int __init kbproc_init ( void )
 {
   int i ;
   printk (KERN_INFO "kbproc : Iniciando ...\n");
   printk (KERN_INFO "kbproc : fg_console es %x\n" , fg_console);
   for (i=0; i<MAX_NR_CONSOLES; i++) 
   {
      if (!vc_cons[i].d)
            break ;
      printk(KERN_INFO "kbproc:poet_atkm: console[%i/%i] #%i , tty %lx\n" , i ,MAX_NR_CONSOLES, vc_cons[i].d->vc_num,(unsigned long) vc_cons[i].d->vc_tty);
    }
    printk(KERN_INFO "kbproc: finished scanning consoles\n");

    my_driver = vc_cons[fg_console].d->vc_tty->driver;
    printk(KERN_INFO "kbproc: tty driver magic %x\n", my_driver->magic);

    /* creamos el archivo en / proc */
    Our_Proc_File = create_proc_entry (PROCFS_NAME, 0644, NULL);
    if ( Our_Proc_File == NULL ) {
       printk (KERN_ALERT "kbproc: Error: No se pudo inicializar /proc/%s\n", PROCFS_NAME ) ;
       return -ENOMEM ;
    }

    /* Configuramos / proc */
    Our_Proc_File->read_proc   = procfile_read ;
    Our_Proc_File->write_proc  = procfile_write ;
    Our_Proc_File->owner       = THIS_MODULE ;
    Our_Proc_File->mode        = S_IFREG | S_IRUGO ;
    Our_Proc_File->uid         = 0;
    Our_Proc_File->gid         = 0;
    Our_Proc_File->size        = 37;
    printk(KERN_INFO "kbproc: Se creo /proc/%s\n", PROCFS_NAME);
    return 0; /* todo ok */
 }

 /* Funcion de descarga del mdulo */
 static void __exit kbproc_cleanup ( void )
 {
    printk ( KERN_INFO " kbproc : Finalizando ...\n");

    /* Restauramos Leds */
    (my_driver->ops->ioctl)(vc_cons[fg_console].d->vc_tty, NULL, KDSETLED, RESTORE_LEDS);

    /* Eliminamos archivo / proc */
    remove_proc_entry(PROCFS_NAME, NULL);
    printk(KERN_INFO "kbproc: Se elimino /proc/%s\n", PROCFS_NAME);
 }
 /*
   * Definimos cuales son las funciones a llamar
 * cuando se carga o descarga el modulo
   */
module_init(kbproc_init);
module_exit(kbproc_cleanup);


