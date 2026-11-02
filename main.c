#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

/* Serial port (USART2) control registers: special memory addresses */
#define RCC_APB1ENR (*(volatile uint32_t *)0x40023840)
#define USART2_SR   (*(volatile uint32_t *)0x40004400)
#define USART2_DR   (*(volatile uint32_t *)0x40004404)
#define USART2_CR1  (*(volatile uint32_t *)0x4000440C)

/* Send one character */
void send_char(char c)
{
    while ((USART2_SR & (1 << 7)) == 0) { }   /* wait until port is ready */
    USART2_DR = c;                            /* send the character */
}

/* Send a whole message */
void send_text(const char *text)
{
    while (*text != 0)
    {
        send_char(*text);
        text++;
    }
}

/* Task A: prints a message, then sleeps for 500 ms, forever */
void task_a(void *param)
{
    (void)param;
    while (1)
    {
        send_text("Task A running\r\n");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* Task B: prints a message, then sleeps for 800 ms, forever */
void task_b(void *param)
{
    (void)param;
    while (1)
    {
        send_text("   Task B running\r\n");
        vTaskDelay(pdMS_TO_TICKS(800));
    }
}

int main(void)
{
    RCC_APB1ENR |= (1 << 17);             /* give clock to USART2 */
    USART2_CR1 = (1 << 13) | (1 << 3);    /* turn serial port on, enable transmit */

    send_text("FreeRTOS started\r\n");

    xTaskCreate(task_a, "A", 256, NULL, 1, NULL);   /* create task A */
    xTaskCreate(task_b, "B", 256, NULL, 1, NULL);   /* create task B */

    vTaskStartScheduler();                /* hand control to FreeRTOS */

    while (1) { }                         /* never reached */
}

/* FreeRTOS calls this if a task uses more memory than it was given */
void vApplicationStackOverflowHook(TaskHandle_t t, char *name)
{
    (void)t;
    (void)name;
    send_text("STACK OVERFLOW\r\n");
    while (1) { }
}

/* FreeRTOS calls this if it runs out of memory for tasks */
void vApplicationMallocFailedHook(void)
{
    send_text("MALLOC FAILED\r\n");
    while (1) { }
}