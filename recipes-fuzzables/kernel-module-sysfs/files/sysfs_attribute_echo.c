#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/string.h>
#include <linux/slab.h>

static struct kobject *example_kobj;
static char *stored_data;

static ssize_t input_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s", stored_data);
}

static ssize_t input_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    kfree(stored_data);
    stored_data = kstrdup(buf, GFP_KERNEL);
    return count;
}

static ssize_t output_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s", stored_data);
}

static ssize_t output_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    return -EPERM; // Do not allow writing to the output file
}

static struct kobj_attribute input_attribute = __ATTR(input, 0664, input_show, input_store);
static struct kobj_attribute output_attribute = __ATTR(output, 0444, output_show, output_store);

static struct attribute *attrs[] = {
    &input_attribute.attr,
    &output_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attrs,
};

static int __init example_init(void)
{
    int retval;

    stored_data = kstrdup("initial data", GFP_KERNEL);

    example_kobj = kobject_create_and_add("sysfs_attribute_echo", kernel_kobj);
    if (!example_kobj)
        return -ENOMEM;

    retval = sysfs_create_group(example_kobj, &attr_group);
    if (retval)
        kobject_put(example_kobj);

    return retval;
}

static void __exit example_exit(void)
{
    kobject_put(example_kobj);
    kfree(stored_data);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module creating two sysfs files for reading and writing");
MODULE_AUTHOR("ejaaskel");

