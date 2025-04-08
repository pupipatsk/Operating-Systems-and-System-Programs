#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>  // for copy_to_user
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("CPU Info Device Driver");

#define DEV_NAME "cpuinfo"

static int major;
static char cpuinfo_buffer[512];

static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
                                unsigned int *ecx, unsigned int *edx) {
    asm volatile("cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "0" (*eax), "2" (*ecx)
        : "memory");
}

static int cpuinfo_open(struct inode *inode, struct file *file) {
    unsigned eax, ebx, ecx, edx;
    char vendor[13];
    int len = 0;

    // Vendor ID
    eax = 0;
    ecx = 0;
    native_cpuid(&eax, &ebx, &ecx, &edx);

    *(unsigned int*)&vendor[0] = ebx;
    *(unsigned int*)&vendor[4] = edx;
    *(unsigned int*)&vendor[8] = ecx;
    vendor[12] = '\0';

    len += snprintf(cpuinfo_buffer + len, sizeof(cpuinfo_buffer) - len,
                    "Vendor ID: %s\n", vendor);

    // Features
    eax = 1;
    native_cpuid(&eax, &ebx, &ecx, &edx);

    len += snprintf(cpuinfo_buffer + len, sizeof(cpuinfo_buffer) - len,
                    "Stepping: %d\nModel: %d\nFamily: %d\nProcessor Type: %d\nExtended Model: %d\nExtended Family: %d\n",
                    eax & 0xF, (eax >> 4) & 0xF, (eax >> 8) & 0xF,
                    (eax >> 12) & 0x3, (eax >> 16) & 0xF, (eax >> 20) & 0xFF);

    // Serial Number
    eax = 3;
    native_cpuid(&eax, &ebx, &ecx, &edx);

    len += snprintf(cpuinfo_buffer + len, sizeof(cpuinfo_buffer) - len,
                    "Serial Number: 0x%08x%08x\n", edx, ecx);

    return 0;
}

static ssize_t cpuinfo_read(struct file *filp, char *buffer, size_t len, loff_t *offset) {
    int bytes_read;

    if (*offset >= strlen(cpuinfo_buffer))
        return 0;

    bytes_read = strlen(cpuinfo_buffer) - *offset;
    if (len < bytes_read)
        bytes_read = len;

    if (copy_to_user(buffer, cpuinfo_buffer + *offset, bytes_read))
        return -EFAULT;

    *offset += bytes_read;
    return bytes_read;
}

static int cpuinfo_release(struct inode *inode, struct file *file) {
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = cpuinfo_open,
    .read = cpuinfo_read,
    .release = cpuinfo_release,
};

static int __init cpuinfo_init(void) {
    major = register_chrdev(0, DEV_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "CPUInfo load failed: %d\n", major);
        return major;
    }
    printk(KERN_INFO "CPUInfo module loaded with major number %d\n", major);
    printk(KERN_INFO "To create device: mknod /dev/%s c %d 0\n", DEV_NAME, major);
    return 0;
}

static void __exit cpuinfo_exit(void) {
    unregister_chrdev(major, DEV_NAME);
    printk(KERN_INFO "CPUInfo module unloaded\n");
}

module_init(cpuinfo_init);
module_exit(cpuinfo_exit);
