#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/timekeeping.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define PROC_NAME "timer"

static struct timespec64 prev_time;
static bool time_initialized = false;

static int timer_show(struct seq_file *m, void *v) {
    struct timespec64 now;
    ktime_get_real_ts64(&now);

    if (time_initialized) {
        struct timespec64 delta = timespec64_sub(now, prev_time);
        seq_printf(m, "current time: %lld.%09ld\n", (long long)now.tv_sec, now.tv_nsec);
        seq_printf(m, "elapsed time: %lld.%09ld\n", (long long)delta.tv_sec, delta.tv_nsec);
    } else {
        seq_printf(m, "current time: %lld.%09ld\n", (long long)now.tv_sec, now.tv_nsec);
        seq_puts(m, "elapsed time: N/A\n");
        time_initialized = true;
    }

    prev_time = now;

    return 0;
}

static int timer_open(struct inode *inode, struct file *file) {
    return single_open(file, timer_show, NULL);
}

static const struct proc_ops timer_fops = {
    .proc_open = timer_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init timer_init(void) {
    proc_create(PROC_NAME, 0, NULL, &timer_fops); 
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

static void __exit timer_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

module_init(timer_init);
module_exit(timer_exit);
