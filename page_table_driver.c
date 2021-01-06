#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "page_table_driver.h"

#define MAJOR_NUM 200
#define MODULE_NAME "OS_Memory_Practice"

static int drv_open(struct inode *inode, struct file *filp)
{
	printk("Device open\n");
	return 0;
}

static int drv_release(struct inode *inode, struct file *filp)
{
	printk("Device close\n");
	return 0;
}

static long drv_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    struct ioctl_arg data;
    memset(&data, 0, sizeof(data));
    switch (cmd)
    {
		case IOCTL_REQUEST:
			// Copy request data from user
			if (copy_from_user(&data, (int __user *)arg, sizeof(data)))
			{
				ret = -EFAULT;
				goto done;
			}
			// Do request
			switch (data.request[0])
			{
				case IO_CR3:	// Read cr3 value
					data.ret = (uint64_t)current->mm->pgd - PAGE_OFFSET;
					break;
				case IO_READ:	// Read physical address value
					data.ret = *(uint64_t*)(data.request[1] + PAGE_OFFSET);
					break;
				case IO_WRITE:	// Write physical address value
					*(uint64_t *)(data.request[1] + PAGE_OFFSET) = data.request[2];
					break;
			}
			// Copy return value to user
			if (copy_to_user((int __user *)arg, &data, sizeof(data)))
			{
				ret = -EFAULT;
				goto done;
			}
			break;
		default:
			ret = -ENOTTY;
			break;
    }

done:
    return ret;
}

struct file_operations drv_fops =
	{
		open : drv_open,
		release : drv_release,
		unlocked_ioctl : drv_unlocked_ioctl
	};

static int os_init(void)
{
	if (register_chrdev(MAJOR_NUM, MODULE_NAME, &drv_fops) < 0)
	{
		printk("%s: Cannot register chrdev %d\n", MODULE_NAME, MAJOR_NUM);
		return (-EBUSY);
	}

	printk("%s: Module installed\n", MODULE_NAME);
	return 0;
}

static void os_exit(void)
{
	unregister_chrdev(MAJOR_NUM, MODULE_NAME);
	printk("%s: Module removed\n", MODULE_NAME);
}

module_init(os_init);
module_exit(os_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("OS - Memory Practice");
