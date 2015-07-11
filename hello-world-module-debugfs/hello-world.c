#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");

static const char *msg = "75d9c1b2b032";
static const size_t len_msg = 12;

static struct dentry *dir;

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

	int res = length;

	if (count != length) {
		res = -EFAULT;
		goto error;
	}

	if (strncmp(kbuff, msg, len_msg) != 0)
		res = -EINVAL;

error:
	kfree(kbuff);
	return res;
}

const struct file_operations id_fops = {
	.read = device_read,
	.write = device_write,
};

static ssize_t read_jiffies(struct file *filp, char *buffer,
	size_t length, loff_t *offset)
{
	int len = 32;
	char buf[len];
	int res = scnprintf(buf, len, "%lu\n", jiffies);

	return simple_read_from_buffer(buffer, length, offset, buf, res);
}

const struct file_operations jiffies_fops = {
	.read = read_jiffies
};

static int __init my_init_module(void)
{
	pr_info("Hello World!");

	dir = debugfs_create_dir("eudyptula", NULL);
	if (!dir)
		return -EPERM;

	struct dentry *id_file = NULL;

	id_file = debugfs_create_file("id", 0666, dir, NULL,  &id_fops);
	if (!id_file)
		return -EPERM;

	struct dentry *jiffies_file = NULL;

	jiffies_file = debugfs_create_file("jiffies", 0444, dir, NULL,
		&jiffies_fops);
	if (!jiffies_file)
		return -EPERM;

	return 0;
}

static void __exit my_exit_module(void)
{
	pr_info("Bye World!");
	debugfs_remove_recursive(dir);
}

module_init(my_init_module);
module_exit(my_exit_module);

