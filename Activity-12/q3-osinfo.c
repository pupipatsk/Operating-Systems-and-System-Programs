#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KRERK PIROMSOPA, PH.D.");
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

static const char f_data1[] =
    "0:CP ENG CU OS 2022S2 - Students, Group Name: แย่แล้วว่ะ\n"
    "1:\t6530113921 Naron Chatjitakornkul\n"
    "2:\t6532126421 Pongpak Manoret\n"
    "3:\t6532142421 Pupipat Singkhorn\n";

// Function prototypes
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);

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
    printk(KERN_INFO "To create device files:\n");
    printk(KERN_INFO "\tmknod /dev/osinfo c %d 0\n", dev_major);
    printk(KERN_INFO "\tmknod /dev/osinfo1 c %d 1\n", dev_major);
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
    int minor = MINOR(inode->i_rdev);
    printk(KERN_INFO "dev minor %d\n", minor);

    if (minor == 0)
        f_ptr = (char *)f_data0;
    else if (minor == 1)
        f_ptr = (char *)f_data1;
    else
        f_ptr = "";

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
