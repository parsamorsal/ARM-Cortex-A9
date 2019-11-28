/************************************
desc : this function will return current value of clock cycle counter register 

in   : _

out  : current value of clock cycle counter register 
************************************/
unsigned int Ret_Time(void)
{
    unsigned int time = 0;
    asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(time));
    return time;
}
