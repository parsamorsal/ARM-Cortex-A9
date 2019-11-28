#define ARMV7_PMCR_E       (1 << 0) /* Enable all counters */
#define ARMV7_PMCR_P       (1 << 1) /* Reset all counters */
#define ARMV7_PMCR_C       (1 << 2) /* Cycle counter reset */
#define ARMV7_PMCR_D       (1 << 3) /* Cycle counts every 64th cpu cycle */
#define ARMV7_PMCR_X       (1 << 4) /* Export to ETM */
#define ARMV7_PMCNTENSET_C (1 << 31) /* Enable cycle counter */
#define ARMV7_PMOVSR_C     (1 << 31) /* Overflow bit */

/***********************************
desc : this function will initialize timer

in   : _

out  : _
***********************************/
noinline void arm_v7_timing_init(void)
{
  int value = 0;
  unsigned int x = 0;

  value |= ARMV7_PMCR_E; // Enable all counters
  value |= ARMV7_PMCR_P; // Reset all counters
  value |= ARMV7_PMCR_C; // Reset cycle counter to zero
  value |= ARMV7_PMCR_X; // Enable export of events

   //value |= ARMV7_PMCR_D; // Enable div64
  

  // Performance Monitor Control Register
  asm volatile ("MCR p15, 0, %0, c9, c12, 0" :: "r" (value));

  // Count Enable Set Register
  value = 0;
  value |= ARMV7_PMCNTENSET_C;

  for (x = 0; x < 4; x++) {
    value |= (1 << x); // Enable the PMx event counter
  }

  asm volatile ("MCR p15, 0, %0, c9, c12, 1" :: "r" (value));

  // Overflow Flag Status register
  value = 0;
  value |= ARMV7_PMOVSR_C;

  for (x = 0; x < 4; x++) {
    value |= (1 << x); // Enable the PMx event counter
  }
  asm volatile ("MCR p15, 0, %0, c9, c12, 3" :: "r" (value));
}
