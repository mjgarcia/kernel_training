#include <linux/module.h>
#include <linux/usb.h>
#include <linux/hid.h>

MODULE_LICENSE("GPL");

static struct usb_device_id usb_keyboard_table[] = {
	{ USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
			USB_INTERFACE_SUBCLASS_BOOT,
			USB_INTERFACE_PROTOCOL_KEYBOARD)},
	{}
};
MODULE_DEVICE_TABLE(usb, usb_keyboard_table);

static int __init my_init_module(void)
{
	pr_info("Hello World!\n");
	return 0;
}

static void __exit my_exit_module(void)
{
	pr_info("Bye World!");
}

module_init(my_init_module);
module_exit(my_exit_module);
