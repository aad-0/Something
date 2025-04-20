/**
 *
 *
 */

#ifndef UARTSLAVEACCEL_H
#define UARTSLAVEACCEL_H
#include "uartSlave.h"

#include "lowpass.h"

#define UARTSLAVE_ACCEL_FLAG_TX	(0x2U)

typedef struct uart_slave_message_s
{
	uint8_t u8Heading;
	uint16_t u16SlaveId;
	uint8_t u8Command;
	uint32_t u32Tick;
	uint16_t u16Encoder;
//	uint16_t u16AxisX;
//	uint16_t u16AxisY;
//	uint16_t u16AxisZ;
		float u16AxisX;
		float u16AxisY;
		float u16AxisZ;
	// CRC
	// endChar
} UartSlaveMessage_TypeDef;


typedef struct uart_slave_accel_s
{
	UartSlave_TypeDef UartSlaveInstance;
	uint32_t u32AxisMask;
	int16_t pi16AccelXYZ [3];

	Lowpass_TypeDef LowpassAccelX;
	Lowpass_TypeDef LowpassAccelY;
	Lowpass_TypeDef LowpassAccelZ;

} UartSlaveAccel_TypeDef;

typedef enum uart_slave_accel_commands_e
{
	UARTSLAVEACCEL_COMMAND_SETAXIS = 0x01U,

} UartSlaveAccelCommands_TypeDef;

typedef enum uart_slave_accel_axis_e
{
	UARTSLAVEACCEL_AXISMASK_GETX = 0x01U,
	UARTSLAVEACCEL_AXISMASK_GETY = 0x02U,
	UARTSLAVEACCEL_AXISMASK_GETZ = 0x04U,

} UartSlaveAccelAxis_TypeDef;


int32_t UartSlaveAccel_Init (UartSlaveAccel_TypeDef * pInit, UartSlaveIo_TypeDef * pIoContext, uint16_t SlaveId);

#endif // UARTSLAVEACCEL_H
