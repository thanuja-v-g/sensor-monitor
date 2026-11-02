#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* Chip and timing */
#define configCPU_CLOCK_HZ                 ( 168000000UL )
#define configTICK_RATE_HZ                 ( 1000 )          /* 1 tick = 1 ms */

/* Scheduler behaviour */
#define configUSE_PREEMPTION               1
#define configUSE_IDLE_HOOK                0
#define configUSE_TICK_HOOK                0
#define configMAX_PRIORITIES               ( 5 )
#define configMINIMAL_STACK_SIZE           ( ( unsigned short ) 128 )   /* in words (4 bytes each) */
#define configMAX_TASK_NAME_LEN            ( 16 )
#define configUSE_16_BIT_TICKS             0

/* Memory */
#define configSUPPORT_DYNAMIC_ALLOCATION   1
#define configTOTAL_HEAP_SIZE              ( ( size_t ) ( 20 * 1024 ) )

/* Features we use */
#define configUSE_MUTEXES                  1
#define configUSE_COUNTING_SEMAPHORES      0
#define configQUEUE_REGISTRY_SIZE          0

/* Safety checks (very useful while learning) */
#define configCHECK_FOR_STACK_OVERFLOW     2
#define configUSE_MALLOC_FAILED_HOOK       1

/* Functions we want to be able to call */
#define INCLUDE_vTaskDelay                 1
#define INCLUDE_vTaskDelete                1
#define INCLUDE_vTaskDelayUntil            1

/* Cortex-M interrupt priority settings (leave as they are) */
#define configPRIO_BITS                    4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5
#define configKERNEL_INTERRUPT_PRIORITY    ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << ( 8 - configPRIO_BITS ) )

/* Connect FreeRTOS handlers to the names in our vector table */
#define vPortSVCHandler     SVC_Handler
#define xPortPendSVHandler  PendSV_Handler
#define xPortSysTickHandler SysTick_Handler

#endif