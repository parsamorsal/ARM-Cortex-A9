/***********************************
desc : flushing a memory cell from cache

in   : virtual_address (virtual address of target memory location)
***********************************/
asm volatile("mcr p15, 0, %0, c7, c14, 1"::"r"(virtual_address));
