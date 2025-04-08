#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h> // for_each_process()

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("SysInfo Device Driver");

#define DEVICE_NAME "cpsysinfo"

static int major;
static char ps_buffer[4096];

static int device_open(struct inode *inode, struct file *file) {
    struct task_struct *task;
    int len = 0;

    int minor = MINOR(inode->i_rdev);
    printk(KERN_INFO "cpsysinfo: open called, minor=%d\n", minor);

    if (minor == 0) {
        // List processes
        for_each_process(task) {
            len += snprintf(ps_buffer + len, sizeof(ps_buffer) - len,
                            "%d,%s\n", task->pid, task->comm);
            if (len >= sizeof(ps_buffer))
                break;
        }
    } else {
        snprintf(ps_buffer, sizeof(ps_buffer), "Invalid minor number.\n");
    }

    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t len, loff_t *offset) {
    int bytes_read;

    if (*offset >= strlen(ps_buffer))
        return 0;

    bytes_read = strlen(ps_buffer) - *offset;
    if (len < bytes_read)
        bytes_read = len;

    if (copy_to_user(buffer, ps_buffer + *offset, bytes_read))
        return -EFAULT;

    *offset += bytes_read;
    return bytes_read;
}

static int device_release(struct inode *inode, struct file *file) {
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = device_read,
    .release = device_release,
};

static int __init cpsysinfo_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "cpsysinfo: failed to register device\n");
        return major;
    }

    printk(KERN_INFO "cpsysinfo: loaded with major number %d\n", major);
    printk(KERN_INFO "Create device with: mknod /dev/cp-psinfo c %d 0\n", major);
    return 0;
}

static void __exit cpsysinfo_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "cpsysinfo: unloaded\n");
}

module_init(cpsysinfo_init);
module_exit(cpsysinfo_exit);
