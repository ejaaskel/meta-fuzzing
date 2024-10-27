#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  // For copy_from_user
#include <linux/slab.h>
#include <linux/cdev.h>     // For cdev structure
#include <linux/device.h>   // For device_create and class_create
#include <linux/mutex.h>    // For locking

#include "linux/ioctl_string_parse.h"

// Declare pre-allocated buffer pool
static char *buffer_pool[POOL_SIZE];
static bool buffer_used[POOL_SIZE];  // Track whether each buffer is in use
static struct mutex pool_lock;       // Protect access to the buffer pool

static dev_t dev_num;                // To store the dynamically allocated device number (major + minor)
static struct cdev my_cdev;          // Character device structure
static struct class *ioctl_class = NULL;  // Device class
static struct device *ioctl_device = NULL;  // Device struct

// Function prototypes
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static long device_ioctl(struct file *, unsigned int, unsigned long);
static char* get_free_buffer(void);
static void release_buffer(char*);

// File operations structure
static struct file_operations fops = {
	.open = device_open,
	.release = device_release,
	.unlocked_ioctl = device_ioctl,
};

// Get a free buffer from the pool
static char* get_free_buffer(void)
{
	int i;
	mutex_lock(&pool_lock);
	for (i = 0; i < POOL_SIZE; i++) {
		if (!buffer_used[i]) {
			buffer_used[i] = true;
			mutex_unlock(&pool_lock);
			return buffer_pool[i];
		}
	}
	mutex_unlock(&pool_lock);
	return NULL;  // No free buffers available
}

// Release a buffer back to the pool
static void release_buffer(char *buffer)
{
	int i;
	mutex_lock(&pool_lock);
	for (i = 0; i < POOL_SIZE; i++) {
		if (buffer_pool[i] == buffer) {
			buffer_used[i] = false;
			break;
		}
	}
	mutex_unlock(&pool_lock);
}

// Initialize the module
static int __init ioctl_example_init(void)
{
	int ret, i;

	// Initialize mutex
	mutex_init(&pool_lock);

	// Allocate buffer pool
	for (i = 0; i < POOL_SIZE; i++) {
		buffer_pool[i] = kmalloc(BUFFER_SIZE, GFP_KERNEL);
		if (!buffer_pool[i]) {
			printk(KERN_ALERT "Failed to allocate memory for buffer %d\n", i);
			ret = -ENOMEM;
			goto error_alloc;
		}
		buffer_used[i] = false;
	}

	// Dynamically allocate a major number
	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		printk(KERN_ALERT "Failed to allocate major number\n");
		goto error_alloc;
	}

	// Initialize and add the cdev structure
	cdev_init(&my_cdev, &fops);
	my_cdev.owner = THIS_MODULE;

	ret = cdev_add(&my_cdev, dev_num, 1);
	if (ret < 0) {
		printk(KERN_ALERT "Failed to add cdev\n");
		goto error_cdev;
	}

	// Create a device class
	ioctl_class = class_create(CLASS_NAME);
	if (IS_ERR(ioctl_class)) {
		printk(KERN_ALERT "Failed to create device class\n");
		ret = PTR_ERR(ioctl_class);
		goto error_class;
	}

	// Create the device in /dev/
	ioctl_device = device_create(ioctl_class, NULL, dev_num, NULL, DEVICE_NAME);
	if (IS_ERR(ioctl_device)) {
		printk(KERN_ALERT "Failed to create device\n");
		ret = PTR_ERR(ioctl_device);
		goto error_device;
	}

	printk(KERN_INFO "IOCTL example module loaded with major number %d\n", MAJOR(dev_num));
	return 0;

error_device:
	class_destroy(ioctl_class);
error_class:
	cdev_del(&my_cdev);
error_cdev:
	unregister_chrdev_region(dev_num, 1);
error_alloc:
	for (i = 0; i < POOL_SIZE; i++) {
		if (buffer_pool[i]) {
			kfree(buffer_pool[i]);
		}
	}
	return ret;
}

// Cleanup the module
static void __exit ioctl_example_exit(void)
{
	int i;

	// Remove the device
	device_destroy(ioctl_class, dev_num);
	class_destroy(ioctl_class);

	// Remove the cdev structure and free the major/minor numbers
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev_num, 1);

	// Free the buffer pool
	for (i = 0; i < POOL_SIZE; i++) {
		if (buffer_pool[i]) {
			kfree(buffer_pool[i]);
		}
	}

	printk(KERN_INFO "IOCTL example module unloaded\n");
}

// Open function
static int device_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device opened\n");
	return 0;
}

// Release function
static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "Device closed\n");
	return 0;
}

// IOCTL function
static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	char *input_buffer;
	char *token;
	int ret, i;

	// Get a free buffer from the pool
	input_buffer = get_free_buffer();
	if (!input_buffer) {
		printk(KERN_ALERT "No free buffers available\n");
		return -ENOMEM;
	}

	switch (cmd) {
		case IOCTL_CMD_PARSE_STRING:
			// Copy the string from user space
			ret = strncpy_from_user(input_buffer, (char *)arg, BUFFER_SIZE);
			if (ret < 0) {
				printk(KERN_ALERT "Failed to copy string from user space\n");
				release_buffer(input_buffer);
				return -EFAULT;
			}

			input_buffer[BUFFER_SIZE - 1] = '\0';

			i = 0;
			char *cur = input_buffer;
			while ((token = strsep(&cur, ",")) != NULL) {
				if (i == 0) {
					i += 1;
				} else if (i == 1) {
					i += 1;
				} else if (i == 2) {
					i += 1;
				} else if (i == 3) {
					i += 1;
				} else if (i == 4) {
					kfree(cur);
				}
			}

			// Release the buffer back to the pool
			release_buffer(input_buffer);
			break;

		default:
			printk(KERN_ALERT "Invalid IOCTL command\n");
			release_buffer(input_buffer);
			return -EINVAL;
	}

	return 0;
}

// Register module entry and exit points
module_init(ioctl_example_init);
module_exit(ioctl_example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ejaaskel");
MODULE_DESCRIPTION("A simple IOCTL kernel module with memory pooling");
MODULE_VERSION("1.0");

