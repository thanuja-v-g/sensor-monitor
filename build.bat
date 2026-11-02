@echo off
if not exist build mkdir build
set K=FreeRTOS-Kernel
arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -O0 -g -Wall -ffreestanding -nostartfiles --specs=nano.specs --specs=nosys.specs -T linker.ld -I. -I%K%/include -I%K%/portable/GCC/ARM_CM3 main.c startup.c %K%/tasks.c %K%/queue.c %K%/list.c %K%/portable/GCC/ARM_CM3/port.c %K%/portable/MemMang/heap_4.c -o build/hello.elf
if errorlevel 1 (
    echo BUILD FAILED
    exit /b 1
)
arm-none-eabi-size build/hello.elf
echo BUILD OK