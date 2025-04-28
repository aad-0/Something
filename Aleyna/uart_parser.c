#include "ioctl_wrapper.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "comm.h"

#define BUFFER_SIZE 512 // A safe size for receiving buffer

void parseMessage(uint8_t *buffer)
{
    uint8_t heading = ComDef_xu8GetHeading(buffer);
    uint8_t command = ComDef_xu8GetCommand(buffer);
    uint16_t payloadLength = ComDef_xu16GetPayloadLength(buffer);
    uint8_t crc = ComDef_xu8GetCrc(buffer);
    uint8_t end = ComDef_xu8GetEnd(buffer);

    // printf("Parsed Message:\n");
    // printf("Heading: 0x%02X\n", heading);
    // printf("Command: 0x%02X\n", command);
    // printf("Payload Length: %u\n", payloadLength);
    // printf("CRC: 0x%02X\n", crc);
    // printf("End: 0x%02X\n", end);

    if (crc != 0xFA)
    {
        // printf("CRC check failed! Expected 0xFA.\n");
        return;
    }

    if (end != 0xBA)
    {
        // printf("End byte check failed! Expected 0xBA.\n");
        return;
    }

    // printf("Payload (hex): ");
    uint8_t *payload = (uint8_t *)&ComDef_xpu8GetPayload(buffer);
    // for (uint16_t i = 0; i < payloadLength; ++i)
    // {
    //     // printf("%02X ", payload[i]);
    // }
    // printf("\n");

    // Optionally: you can now cast payload into structures depending on the command
    if ((command & ComDefCommandModeMask) == ComDefCommandMode)
    {
        ComDefMode_TypeDef *mode = (ComDefMode_TypeDef *)payload;
        // printf("Mode: %u\n", mode->u8Mode);
    }
    else if ((command & ComDefCommandModeMask) == ComDefCommandAngle)
    {
        ComDefAngle_TypeDef *angle = (ComDefImu_TypeDef *)payload;
        ComDefImu_TypeDef *imu = (ComDefImu_TypeDef *)payload;
        // printf("IMU X: %.2f, Y: %.2f, Z: %.2f\n", imu->fX, imu->fY, imu->fZ);
        fprintf(stdout, "IMU X: %.2f, Y: %.2f, Z: %.2f\n", imu->fX, imu->fY, imu->fZ);
        fflush(stdout);
        // printf("Angle: %u\n", angle->u16Angle);
    }
    else if ((command & ComDefCommandModeMask) == ComDefCommandSamplingRate)
    {
        ComDefImu_TypeDef *imu = (ComDefImu_TypeDef *)payload;
        fprintf(stdout, "IMU X: %.2f, Y: %.2f, Z: %.2f\n", imu->fX, imu->fY, imu->fZ);
        fflush(stdout);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <UART device>\n", argv[0]);
        return -1;
    }

    UartInstance_TypeDef uart;
    if (uart_Init(&uart, argv[1], 115200, 1000) != 0)
    {
        fprintf(stderr, "Failed to initialize UART.\n");
        return -1;
    }

    uartConfigure(&uart);

    uint8_t byte;
    uint8_t buffer[BUFFER_SIZE];
    size_t bufferIndex = 0;
    int payloadSizeExpected = 0;
    int parsing = 0;

    while (1)
    {
        int32_t bytesRead = uartRead(&uart, &byte, 1);
        if (bytesRead == 1)
        {
            if (!parsing)
            {
                if (byte == 0xAA) // Start byte
                {
                    bufferIndex = 0;
                    buffer[bufferIndex++] = byte;
                    parsing = 1;
                }
            }
            else
            {
                buffer[bufferIndex++] = byte;

                if (bufferIndex == 5) // After heading+cmd+length (first 5 bytes)
                {
                    uint16_t payloadLen = ComDef_xu16GetPayloadLength(&buffer[0]);
                    payloadSizeExpected = payloadLen + 5; // (header 3 bytes + payload + CRC + END)
                }

                if (payloadSizeExpected > 0 && bufferIndex >= payloadSizeExpected)
                {
                    // Full message received
                    parseMessage(buffer);
                    parsing = 0; // Ready for next message
                    payloadSizeExpected = 0;
                }
            }
        }
        // usleep(1000); // sleep 1ms to prevent CPU overuse
    }

    uartClose(&uart);
    return 0;
}

