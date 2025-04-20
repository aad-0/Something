#include "ioctl_wrapper.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define UART_MESSAGE_SIZE sizeof(UartSlaveMessage_TypeDef)

typedef struct uart_slave_message_s
{
    uint8_t u8Heading;
    uint16_t u16SlaveId;
    uint8_t u8Command;
    uint32_t u32Tick;
    float u16AxisX;
    float u16AxisY;
    float u16AxisZ;
} UartSlaveMessage_TypeDef;

void parseAndPrintMessage(UartSlaveMessage_TypeDef *msg)
{
    printf("Received Message:\n");
    printf("Heading: 0x%X\n", msg->u8Heading);
    printf("Slave ID: %u\n", msg->u16SlaveId);
    printf("Command: 0x%X\n", msg->u8Command);
    printf("Tick: %u\n", msg->u32Tick);
    printf("Axis X: %f\n", msg->u16AxisX);
    printf("Axis Y: %f\n", msg->u16AxisY);
    printf("Axis Z: %f\n", msg->u16AxisZ);
}

int main(int argc, char ** argv)
{
    UartInstance_TypeDef uart;
    fprintf (stdout, "AAAA : %s\r\n", argv[1]);
    if (uart_Init(&uart, argv[1], 115200, 1000) != 0)
    {
        fprintf(stderr, "Failed to initialize UART.\n");
        return -1;
    }

    uartConfigure(&uart);

    UartSlaveMessage_TypeDef msg;
    while (1)
    {
        int32_t bytesRead = uartRead(&uart, &msg, UART_MESSAGE_SIZE);
        if (bytesRead == UART_MESSAGE_SIZE)
        {
            parseAndPrintMessage(&msg);
        }
        usleep(10000); // Prevent CPU overuse
    }

    uartClose(&uart);
    return 0;
}

