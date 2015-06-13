#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

static ssize_t device_read(struct file *filp, char *buffer,
size_t length, loff_t *offset)
{
	const char *msg = "75d9c1b2b032";
	const size_t len = strlen(msg);

	/* We have copied the whole message*/
	if (*offset == len)
		return 0;

	/* Buffer is not big enough */
	if (length < len)
		return -ENOMEM;

	if (copy_to_user(buffer, msg, len) != 0)
		return -EFAULT;

	*offset = len;
	return len;
}

static ssize_t device_write(struct file *filp, const char *buffer,
size_t length, loff_t *off)
{
	const char *msg = "75d9c1b2b032";
	const size_t len = strlen(msg);

	if (len != length)
		return -EINVAL;

	if (strncmp(buffer, msg, length) != 0)
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

