/**
 *
 *
 *
 */

#include "UartSlaveAccel.h"

#include "stm32f4_discovery.h"
#include "stm32f4_discovery_accelerometer.h"

void UartSlaveAccel_StateMachine ();


int32_t UartSlaveAccel_Init (UartSlaveAccel_TypeDef * pInit, UartSlaveIo_TypeDef * pIoContext, uint16_t SlaveId)
{
	UartSlave_Init (& pInit->UartSlaveInstance, pIoContext, SlaveId);
	pInit->u32AxisMask = UARTSLAVEACCEL_AXISMASK_GETX | UARTSLAVEACCEL_AXISMASK_GETY | UARTSLAVEACCEL_AXISMASK_GETZ;

	lowpass_Init(&pInit->LowpassAccelX, 0.02);
	lowpass_Init(&pInit->LowpassAccelY, 0.02);
	lowpass_Init(&pInit->LowpassAccelZ, 0.02);
	pInit->pi16AccelXYZ [0] = 0;
	pInit->pi16AccelXYZ [1] = 0;
	pInit->pi16AccelXYZ [2] = 0;

	pInit->UartSlaveInstance.pfvStateMachine = UartSlaveAccel_StateMachine;

	return 0;
}


static UartSlaveMessage_TypeDef txMessage;
void UartSlaveAccel_StateMachine (void * pSlaveDevice)
{
	UartSlaveAccel_TypeDef * pDevice = (UartSlaveAccel_TypeDef *) pSlaveDevice;
	// get accels
	BSP_ACCELERO_GetXYZ(& pDevice->pi16AccelXYZ [0]);
	// update the lowpasses
    lowpass_update(&pDevice->LowpassAccelX, (float)pDevice->pi16AccelXYZ[0]);
    lowpass_update(&pDevice->LowpassAccelY, (float)pDevice->pi16AccelXYZ[1]);
    lowpass_update(&pDevice->LowpassAccelZ, (float)pDevice->pi16AccelXYZ[2]);

//    static UartSlaveMessage_TypeDef txMessage;
    txMessage.u8Heading = 0xAA;
    txMessage.u16SlaveId = pDevice->UartSlaveInstance.u16SlaveId;
    txMessage.u8Command = pDevice->UartSlaveInstance.u32CommandMode;
    txMessage.u32Tick = HAL_GetTick();

    txMessage.u16AxisX = pDevice->pi16AccelXYZ[0];
    txMessage.u16AxisY = pDevice->pi16AccelXYZ[1];
    txMessage.u16AxisZ = pDevice->pi16AccelXYZ[2];
////
//    txMessage.u16AxisX = pDevice->LowpassAccelX.out;
//    txMessage.u16AxisY = pDevice->LowpassAccelY.out;
//    txMessage.u16AxisZ = pDevice->LowpassAccelZ.out;


	switch (pDevice->UartSlaveInstance.u32CommandMode)
	{
	case UARTSLAVE_COMMAND_MODE_STREAM:
		if (pDevice->UartSlaveInstance.u32TxFlags && UARTSLAVE_ACCEL_FLAG_TX)
		{
			pDevice->UartSlaveInstance.pIoContext->pfvTx (& txMessage, sizeof(txMessage));
			xvUartSlaveClearTxFlag(&pDevice->UartSlaveInstance, UARTSLAVE_ACCEL_FLAG_TX);
		}
		break;
	case UARTSLAVE_COMMAND_MODE_ONREQUEST:
		///@ todo
		break;

	case UARTSLAVE_COMMAND_MODE_NONE:
	default:
		break;
	}
}
