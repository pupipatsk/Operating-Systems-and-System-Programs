#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRERK PIROMSOPA, PH.D. <Krerk.P@chula.ac.th>");
MODULE_DESCRIPTION("\"osinfo\" Character Device");

#define DEVICENAME "osinfo"

static int dev_major;
static int dev_open = 0;
static char *f_ptr;
static const char f_data0[] =
    "0:CP ENG CU OS 2022S2 - Instructors\n"
    "1:\tVeera Muangsin, Ph.D.\n"
    "2:\tKrerk Piromsopa, Ph.D.\n"
    "3:\tThongchai Rojkangsadan\n";

// Function prototypes
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);

// File operations structure
static struct file_operations dev_fops = {
    .read = device_read,
    .open = device_open,
    .release = device_release
};

int init_module(void) {
    printk(KERN_INFO "CPCHAR: dev osinfo init\n");
    dev_major = register_chrdev(0, DEVICENAME, &dev_fops);

    if (dev_major < 0) {
        printk(KERN_ALERT "Fail register_chrdev osinfo with %d\n", dev_major);
        return dev_major;
    }

    printk(KERN_INFO "Device MajorNumber %d.\n", dev_major);
    printk(KERN_INFO "To create a device file:\n");
    printk(KERN_INFO "\tmknod /dev/%s c %d 0\n", DEVICENAME, dev_major);
    return 0;
}

void cleanup_module(void) {
    printk(KERN_INFO "CPCHAR: dev osinfo cleanup\n");
    unregister_chrdev(dev_major, DEVICENAME);
}

static int device_open(struct inode *inode, struct file *file) {
    if (dev_open)
        return -EBUSY;

    dev_open++;
    printk(KERN_INFO "dev minor %d\n", MINOR(inode->i_rdev));
    f_ptr = (char *)f_data0;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    dev_open--;
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset) {
    int bytes_read = 0;
    if (*f_ptr == 0)
        return 0;

    while (length && *f_ptr) {
        put_user(*(f_ptr++), buffer++);
        length--;
        bytes_read++;
    }

    return bytes_read;
}