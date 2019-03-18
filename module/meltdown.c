#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/mm.h>
#include<linux/mm_types.h>
#include<linux/file.h>
#include<linux/fs.h>
#include<linux/path.h>
#include<linux/slab.h>
#include<linux/dcache.h>
#include<linux/sched.h>
#include<linux/uaccess.h>
#include<linux/fs_struct.h>
#include <asm/tlbflush.h>
#include<linux/device.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

static char secret = 'X';
static char* secret_buffer;

MODULE_DESCRIPTION("Meltdown modules");

static struct proc_dir_entry *proc_entry;

static int proc_open(struct inode *inode, struct file *file)
{

   return single_open(file, NULL, PDE_DATA(inode));

}

static ssize_t proc_read(struct file *file_, char *buff, 
                         size_t length, loff_t *offset)
{
   memcpy(secret_buffer, &secret, 1);              
   return 1;
}

static const struct file_operations proc_fops =
{
//    .owner = THIS_MODULE,
   .open = proc_open,
   .read = proc_read,
//    .llseek = seq_lseek,
//    .release = single_release,
};

static int __init meltdown_init(void)
{
    printk(KERN_INFO "Hello world!\n");
    printk("Secret address:%p\n", &secret);
    // remove_proc_entry("secret", NULL);
    
    proc_entry = proc_create_data("secret",0,NULL,&proc_fops,NULL);
    secret_buffer = (char*) kmalloc(sizeof(char),GFP_KERNEL);
    if (secret_buffer && proc_entry)
        return 0;   
    return -ENOMEM;
}

static void __exit meltdown_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
    if(secret_buffer) kfree(secret_buffer);
    remove_proc_entry("secret", NULL);
    

}

module_init(meltdown_init);
module_exit(meltdown_cleanup);


/* Reference 

int single_open(struct file *file, int (*show)(struct seq_file *, void *),
		void *data)
{
	struct seq_operations *op = kmalloc(sizeof(*op), GFP_KERNEL);
	int res = -ENOMEM;

	if (op) {
		op->start = single_start;
		op->next = single_next;
		op->stop = single_stop;
		op->show = show;
		res = seq_open(file, op);
		if (!res)
			((struct seq_file *)file->private_data)->private = data;
		else
			kfree(op);
	}
	return res;
}

*/