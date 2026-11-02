#include <stdint.h>

/* These names come from linker.ld */
extern uint32_t _estack;   /* top of RAM */
extern uint32_t _sidata;   /* where initial values of variables sit in flash */
extern uint32_t _sdata;    /* start of variables in RAM */
extern uint32_t _edata;    /* end of variables in RAM */
extern uint32_t _sbss;     /* start of zero-filled variables */
extern uint32_t _ebss;     /* end of zero-filled variables */

int main(void);
void Reset_Handler(void);

/* If anything goes wrong, the chip lands here and waits */
void Default_Handler(void)
{
    while (1) { }
}

/* FreeRTOS provides these three. Until then they point at Default_Handler. */
void SVC_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

/* The chip reads this table first: [0] = stack top, [1] = where to start */
__attribute__((section(".isr_vector"), used))
void *const vector_table[16] = {
    &_estack,                 /* initial stack pointer */
    (void *)Reset_Handler,    /* reset: first code to run */
    (void *)Default_Handler,  /* NMI */
    (void *)Default_Handler,  /* HardFault */
    (void *)Default_Handler,  /* MemManage */
    (void *)Default_Handler,  /* BusFault */
    (void *)Default_Handler,  /* UsageFault */
    0, 0, 0, 0,
    (void *)SVC_Handler,      /* SVCall */
    (void *)Default_Handler,  /* DebugMon */
    0,
    (void *)PendSV_Handler,   /* PendSV */
    (void *)SysTick_Handler   /* SysTick */
};

void Reset_Handler(void)
{
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;

    /* 1. copy variables with initial values from flash to RAM */
    while (dst < &_edata)
    {
        *dst++ = *src++;
    }

    /* 2. set all other variables to zero */
    for (dst = &_sbss; dst < &_ebss; dst++)
    {
        *dst = 0;
    }

    /* 3. run your program */
    main();

    while (1) { }
}