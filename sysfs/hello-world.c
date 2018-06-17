#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");

static const char *msg = "75d9c1b2b032";
static const size_t len_msg = 12;

static struct kobject *example_kobj;

static char foo_buf[PAGE_SIZE];

DEFINE_MUTEX(foo_mutex);

static int id;
static int foo;

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr,
	char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s", msg);
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
	const char *buf, size_t count)
{
	if (len_msg != count)
		return -EINVAL;

	if (strncmp(buf, msg, count) != 0)
		return -EINVAL;

	return count;
}

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr,
	char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%lu", jiffies);
}

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
	char *buf)
{
	int res = 0;

	if (mutex_lock_interruptible(&foo_mutex))
		return -EINTR;

	res = snprintf(buf, PAGE_SIZE, "%s", foo_buf);

	mutex_unlock(&foo_mutex);

	return res;
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
	const char *buf, size_t count)
{
	int res = 0;

	if (mutex_lock_interruptible(&foo_mutex))
		return -EINTR;

	res = snprintf(foo_buf, PAGE_SIZE, "%s", buf);

	mutex_unlock(&foo_mutex);

	return res;
}

static struct kobj_attribute id_attribute =
	__ATTR(id, 0666, id_show, id_store);

static struct kobj_attribute foo_attribute =
	__ATTR(foo, 0644, foo_show, foo_store);

static struct kobj_attribute jiffies_attribute = {
	.attr = {
		.name = "jiffies",
		.mode = 0444,
	},
	.show = jiffies_show,
	.store = NULL,
};

static struct attribute *attrs[] = {
	&id_attribute.attr,
	&jiffies_attribute.attr,
	&foo_attribute.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init my_init_module(void)
{
	int retval = 0;

	pr_info("Hello World!");

	example_kobj = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!example_kobj)
		return -ENOMEM;

	retval = sysfs_create_group(example_kobj, &attr_group);
	if (retval)
		kobject_put(example_kobj);

	return retval;
}

static void __exit my_exit_module(void)
{
	pr_info("Bye World!");
	kobject_put(example_kobj);
}

module_init(my_init_module);
module_exit(my_exit_module);

