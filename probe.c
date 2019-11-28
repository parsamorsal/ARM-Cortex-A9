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
