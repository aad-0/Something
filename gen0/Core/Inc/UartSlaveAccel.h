/**
 *
 *
 */

#ifndef UARTSLAVEACCEL_H
#define UARTSLAVEACCEL_H
#include "uartSlave.h"

#include "lowpass.h"

#define UARTSLAVE_ACCEL_FLAG_TX	(0x2U)


typedef struct uart_slave_accel_s
{
	UartSlave_TypeDef UartSlaveInstance;
	int16_t pi16AccelXYZ [3];

	Lowpass_TypeDef LowpassAccelX;
	Lowpass_TypeDef LowpassAccelY;
	Lowpass_TypeDef LowpassAccelZ;
} UartSlaveAccel_TypeDef;


int32_t UartSlaveAccel_Init (UartSlaveAccel_TypeDef * pInit, UartSlaveIo_TypeDef * pIoContext, uint16_t SlaveId);

#endif // UARTSLAVEACCEL_H
