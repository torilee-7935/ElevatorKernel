#include <linux/seq_file.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("chatgjt");
MODULE_DESCRIPTION("Elevator Module");

#define MAX_PASSENGERS 5
#define MAX_WEIGHT 14 
#define ELEVATOR_OFFLINE 0
#define ELEVATOR_IDLE 1
#define ELEVATOR_LOADING 2
#define ELEVATOR_UP 3
#define ELEVATOR_DOWN 4

int start_elevator(void);                                                           
int issue_request(int start_floor, int destination_floor, int type);                
int stop_elevator(void);                                                            

extern int (*STUB_start_elevator)(void);
extern int (*STUB_issue_request)(int,int,int);
extern int (*STUB_stop_elevator)(void);

struct passenger {
 int type;
 int start_floor;
 int dest_floor;
 int weight; 
 struct list_head list;
};

struct floor {
 int floor_num;
 struct list_head waiting_list;
};

static struct floor floors[5];
static struct list_head elevator_passengers;
static int current_floor = 0;
static int elevator_state = ELEVATOR_OFFLINE;
static struct task_struct *elevator_thread;
static struct mutex elevator_mutex;
static int total_weight = 0; 
static int total_passengers_waiting = 0;
static int total_passengers_serviced = 0;

static const int passenger_weights[] = {2, 3, 4, 1}; 

static struct proc_dir_entry *proc_entry;

static inline int list_len(struct list_head *head) {
 struct list_head *pos;
 int count = 0;
 list_for_each(pos, head) {
     count++;
 }
 return count;
}

static int elevator_scheduler(void *data) {
while (!kthread_should_stop()) {
    mutex_lock(&elevator_mutex);
    if (elevator_state == ELEVATOR_IDLE) {
        int i;
        for (i = 0; i < 5; i++) {
            if (!list_empty(&floors[i].waiting_list)) {
                elevator_state = ELEVATOR_LOADING;
                current_floor = i;
                break;
            }
        }
    }
    if (elevator_state == ELEVATOR_LOADING) {
        struct passenger *passenger, *tmp;
        int passenger_count = 0;
        list_for_each_entry_safe(passenger, tmp, &floors[current_floor].waiting_list, list) {
            if (passenger_count < MAX_PASSENGERS && total_weight + passenger->weight <= MAX_WEIGHT) {
                total_weight += passenger->weight;
                list_move_tail(&passenger->list, &elevator_passengers);
                passenger_count++;
                total_passengers_waiting--;
            } else {
                break;
            }
        }
        msleep(1000); 
        if (list_empty(&elevator_passengers)) {
            elevator_state = ELEVATOR_IDLE;
        } else {
            elevator_state = ELEVATOR_UP;
        }
    }
    if (elevator_state == ELEVATOR_UP || elevator_state == ELEVATOR_DOWN) {
        struct passenger *passenger, *tmp;
        int dest_floor;
        list_for_each_entry_safe(passenger, tmp, &elevator_passengers, list) {
            dest_floor = passenger->dest_floor;
            if ((elevator_state == ELEVATOR_UP && dest_floor > current_floor) ||
                (elevator_state == ELEVATOR_DOWN && dest_floor < current_floor)) {
                list_del(&passenger->list);
                kfree(passenger);
                total_weight -= passenger->weight;
                total_passengers_serviced++;
                continue;
            }
        }
        if (elevator_state == ELEVATOR_UP) {
            current_floor++;
            if (current_floor == 4) 
                elevator_state = ELEVATOR_DOWN;
        } else {
            current_floor--;
            if (current_floor == 0)
                elevator_state = ELEVATOR_UP;
        }
        msleep(2000); 
    }
    mutex_unlock(&elevator_mutex);
    if (kthread_should_stop())
        break;
    msleep(100); 
}
return 0;
}

int start_elevator(void) {
 if (elevator_state != ELEVATOR_OFFLINE) return 1;
 mutex_lock(&elevator_mutex);
 elevator_state = ELEVATOR_IDLE;
 elevator_thread = kthread_run(elevator_scheduler, NULL, "elevator_thread");
 mutex_unlock(&elevator_mutex);
 if (IS_ERR(elevator_thread)) return -ENOMEM;
 return 0;
}

int issue_request(int start_floor, int dest_floor, int type) {
if (start_floor < 0 || start_floor > 4 || dest_floor < 0 || dest_floor > 4 || type < 0 || type > 3)
    return 1;
struct passenger *new_passenger;
new_passenger = kmalloc(sizeof(struct passenger), GFP_KERNEL);
if (!new_passenger)
    return -ENOMEM;
new_passenger->start_floor = start_floor;
new_passenger->dest_floor = dest_floor;
new_passenger->type = type;
new_passenger->weight = passenger_weights[type];
INIT_LIST_HEAD(&new_passenger->list);
mutex_lock(&elevator_mutex);
list_add_tail(&new_passenger->list, &floors[start_floor].waiting_list);
total_passengers_waiting++;
mutex_unlock(&elevator_mutex);
return 0;
}

int stop_elevator(void) {
 if (elevator_state == ELEVATOR_OFFLINE) return 1;
 mutex_lock(&elevator_mutex);
 elevator_state = ELEVATOR_OFFLINE;
 kthread_stop(elevator_thread);
 mutex_unlock(&elevator_mutex);
 return 0;
}

static int elevator_proc_open(struct inode *inode, struct file *file);

static int elevator_proc_show(struct seq_file *m, void *v) {
 int i;
 struct passenger *passenger;
 seq_printf(m, "Elevator state: %s\n", elevator_state == ELEVATOR_OFFLINE ? "OFFLINE" :
                                         elevator_state == ELEVATOR_IDLE ? "IDLE" :
                                         elevator_state == ELEVATOR_LOADING ? "LOADING" :
                                         elevator_state == ELEVATOR_UP ? "UP" : "DOWN");
 seq_printf(m, "Current floor: %d\n", current_floor);
 seq_printf(m, "Current load: %d.%d lbs\n", total_weight / 2, total_weight % 2);
 seq_puts(m, "Elevator status: ");
 list_for_each_entry(passenger, &elevator_passengers, list) {
     char passenger_type;
     switch (passenger->type) {
         case 0:
             passenger_type = 'P';
             break;
         case 1:
             passenger_type = 'L';
             break;
         case 2:
             passenger_type = 'B';
             break;
         case 3:
             passenger_type = 'V';
             break;
         default:
             passenger_type = '?';
     }
     seq_printf(m, "%c%d ", passenger_type, passenger->dest_floor);
 }
 seq_puts(m, "\n\n");
 for (i = 0; i < 5; i++) {
     seq_printf(m, "%s Floor %d: %d", i == current_floor ? "[*]" : "[ ]", i, total_passengers_waiting - list_len(&floors[i].waiting_list));
     list_for_each_entry(passenger, &floors[i].waiting_list, list) {
         char passenger_type;
         switch (passenger->type) {
             case 0:
                 passenger_type = 'P';
                 break;
             case 1:
                 passenger_type = 'L';
                 break;
             case 2:
                 passenger_type = 'B';
                 break;
             case 3:
                 passenger_type = 'V';
                 break;
             default:
                 passenger_type = '?';
         }
         seq_printf(m, " %c%d", passenger_type, passenger->dest_floor);
     }
     seq_puts(m, "\n");
 }
 seq_puts(m, "\n");
 seq_printf(m, "Number of passengers: %d\n", list_len(&elevator_passengers));
 seq_printf(m, "Number of passengers waiting: %d\n", total_passengers_waiting);
 seq_printf(m, "Number of passengers serviced: %d\n", total_passengers_serviced);
 return 0;
}

static int elevator_proc_open(struct inode *inode, struct file *file) {
 return single_open(file, elevator_proc_show, NULL);
}

static const struct proc_ops elevator_proc_ops = {
 .proc_open = elevator_proc_open,
 .proc_read = seq_read,
 .proc_lseek = seq_lseek,
 .proc_release = single_release,
};

static int __init elevator_init(void) {
 STUB_start_elevator = start_elevator;
 STUB_issue_request = issue_request;
 STUB_stop_elevator = stop_elevator;
 mutex_init(&elevator_mutex);
 for (int i = 0; i < 5; i++) {
     INIT_LIST_HEAD(&floors[i].waiting_list);
 }
 INIT_LIST_HEAD(&elevator_passengers);
 proc_entry = proc_create("elevator", 0, NULL, &elevator_proc_ops);
 if (!proc_entry) return -ENOMEM;
 return 0;
}

static void __exit elevator_exit(void) {
 if (proc_entry) {
     proc_remove(proc_entry);
 }
 if (elevator_thread) {
     kthread_stop(elevator_thread);
 }
 STUB_start_elevator = NULL;
 STUB_issue_request = NULL;
 STUB_stop_elevator = NULL;
 mutex_destroy(&elevator_mutex);
}

module_init(elevator_init);
module_exit(elevator_exit);

