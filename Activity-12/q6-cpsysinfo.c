#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h> // for_each_process
#include <linux/mm.h>           // global_page_state
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("SysInfo Device Driver");

#define DEVICE_NAME "cpsysinfo"

static int major;
static char info_buffer[8192];

static int device_open(struct inode *inode, struct file *file) {
    int minor = MINOR(inode->i_rdev);
    struct task_struct *task;
    unsigned long mem_total_kb, mem_free_kb, mem_avail_kb;

    memset(info_buffer, 0, sizeof(info_buffer));

    printk(KERN_INFO "cpsysinfo: open called, minor=%d\n", minor);

    if (minor == 0) {
        int len = 0;
        for_each_process(task) {
            len += snprintf(info_buffer + len, sizeof(info_buffer) - len,
                            "%d,%s\n", task->pid, task->comm);
            if (len >= sizeof(info_buffer))
                break;
        }
    } else if (minor == 1) {
        struct sysinfo i;
        si_meminfo(&i);

        mem_total_kb  = i.totalram * i.mem_unit / 1024;
        mem_free_kb   = i.freeram * i.mem_unit / 1024;
        mem_avail_kb  = i.freeram * i.mem_unit / 1024; // Simple estimate

        snprintf(info_buffer, sizeof(info_buffer),
                 "MemTotal: %lu kB\nMemFree: %lu kB\nMemAvailable: %lu kB\n",
                 mem_total_kb, mem_free_kb, mem_avail_kb);
    } else {
        snprintf(info_buffer, sizeof(info_buffer), "Invalid minor number.\n");
    }

    return 0;
}

static ssize_t device_read(struct file *filp, char *buffer, size_t len, loff_t *offset) {
    int bytes_read;

    if (*offset >= strlen(info_buffer))
        return 0;

    bytes_read = strlen(info_buffer) - *offset;
    if (len < bytes_read)
        bytes_read = len;

    if (copy_to_user(buffer, info_buffer + *offset, bytes_read))
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
        printk(KERN_ALERT "cpsysinfo: failed to register\n");
        return major;
    }

    printk(KERN_INFO "cpsysinfo: loaded with major number %d\n", major);
    printk(KERN_INFO "To create devices:\n");
    printk(KERN_INFO "  mknod /dev/cp-psinfo c %d 0\n", major);
    printk(KERN_INFO "  mknod /dev/cp-meminfo c %d 1\n", major);
    return 0;
}

static void __exit cpsysinfo_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "cpsysinfo: unloaded\n");
}

module_init(cpsysinfo_init);
module_exit(cpsysinfo_exit);
