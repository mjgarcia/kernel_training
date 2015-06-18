#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static const char *msg = "75d9c1b2b032";
static const size_t len_msg = 12;

static ssize_t device_read(struct file *filp, char *buffer,
size_t length, loff_t *offset)
{
	return simple_read_from_buffer(buffer, length, offset, msg, len_msg);
}

static ssize_t device_write(struct file *filp, const char *buffer,
size_t length, loff_t *off)
{
	if (len_msg != length)
		return -EINVAL;

	char *kbuff = kmalloc(len_msg, GFP_KERNEL);

	if (!kbuff)
		return -ENOMEM;

	ssize_t count = simple_write_to_buffer(kbuff, len_msg, off,
		buffer, length);

	if (count != length)
		return -EFAULT;

	if (strncmp(kbuff, msg, len_msg) != 0)
		return -EINVAL;

	return length;
}

const struct file_operations fops = {
	.read = device_read,
	.write = device_write,
};

static struct miscdevice my_dev;

static int __init my_init_module(void)
{
	pr_info("Hello World!");
	my_dev.minor = MISC_DYNAMIC_MINOR;
	my_dev.name = "eudyptula";
	my_dev.fops = &fops;
	return misc_register(&my_dev);
}

static void __exit my_exit_module(void)
{
	pr_info("Bye World!");
	misc_deregister(&my_dev);
}

module_init(my_init_module);
module_exit(my_exit_module);

