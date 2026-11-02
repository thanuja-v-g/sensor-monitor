#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

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

/* Send a number as text, for example 42 becomes '4' then '2' */
void send_number(int n)
{
    char digits[10];
    int count = 0;

    if (n == 0)
    {
        send_char('0');
        return;
    }

    while (n > 0)
    {
        digits[count] = '0' + (n % 10);   /* last digit, as a character */
        n = n / 10;                       /* remove the last digit */
        count++;
    }

    while (count > 0)                     /* digits are backwards, so send in reverse */
    {
        count--;
        send_char(digits[count]);
    }
}

/* Fake temperature readings, like a sensor would give (in degrees C) */
int readings[10] = {25, 26, 27, 29, 31, 33, 32, 30, 28, 26};

/* The queue: a safe pipe that carries readings from one task to another */
QueueHandle_t reading_queue;

/* Sampling task: "reads the sensor" every 500 ms and puts the value in the queue */
void sampling_task(void *param)
{
    (void)param;
    int index = 0;

    while (1)
    {
        int temp = readings[index];
        xQueueSend(reading_queue, &temp, 0);   /* put it in the pipe */
        index = (index + 1) % 10;

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/* Processing task: waits for readings, averages the last 4, prints, raises alert */
void processing_task(void *param)
{
    (void)param;
    int window[4] = {0, 0, 0, 0};
    int w = 0;
    int temp;

    while (1)
    {
        /* sleep until a reading arrives in the queue */
        if (xQueueReceive(reading_queue, &temp, portMAX_DELAY) == pdTRUE)
        {
            window[w] = temp;
            w = (w + 1) % 4;

            int average = (window[0] + window[1] + window[2] + window[3]) / 4;

            send_text("temp = ");
            send_number(temp);
            send_text("  average = ");
            send_number(average);

            if (average > 30)
            {
                send_text("  ALERT: too hot");
            }
            send_text("\r\n");
        }
    }
}

int main(void)
{
    RCC_APB1ENR |= (1 << 17);             /* give clock to USART2 */
    USART2_CR1 = (1 << 13) | (1 << 3);    /* turn serial port on, enable transmit */

    send_text("Sensor pipeline started\r\n");

    reading_queue = xQueueCreate(8, sizeof(int));   /* room for 8 readings */

    xTaskCreate(sampling_task,   "sample",  256, NULL, 2, NULL);   /* priority 2 */
    xTaskCreate(processing_task, "process", 256, NULL, 1, NULL);   /* priority 1 */

    vTaskStartScheduler();

    while (1) { }
}

void vApplicationStackOverflowHook(TaskHandle_t t, char *name)
{
    (void)t;
    (void)name;
    send_text("STACK OVERFLOW\r\n");
    while (1) { }
}

void vApplicationMallocFailedHook(void)
{
    send_text("MALLOC FAILED\r\n");
    while (1) { }
}