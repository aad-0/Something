/**
 *
 *
 *
 */

#include "UartSlaveAccel.h"

#include "stm32f4_discovery.h"
#include "stm32f4_discovery_accelerometer.h"

#include "comm.h"

void UartSlaveAccel_StateMachine ();


int32_t UartSlaveAccel_Init (UartSlaveAccel_TypeDef * pInit, UartSlaveIo_TypeDef * pIoContext, uint16_t SlaveId)
{
	UartSlave_Init (& pInit->UartSlaveInstance, pIoContext, SlaveId);

	lowpass_Init(&pInit->LowpassAccelX, 0.02);
	lowpass_Init(&pInit->LowpassAccelY, 0.02);
	lowpass_Init(&pInit->LowpassAccelZ, 0.02);
	pInit->pi16AccelXYZ [0] = 0;
	pInit->pi16AccelXYZ [1] = 0;
	pInit->pi16AccelXYZ [2] = 0;

	pInit->UartSlaveInstance.pfvStateMachine = UartSlaveAccel_StateMachine;

	return 0;
}

static ComDef_xpu8DeclareBuffer(txMessage, ComDefImu_TypeDef);

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
    ComDef_xu8GetHeading(&txMessage) = 0xAA;
    ComDef_xu8GetCommand(&txMessage) = ComDef_xu8CommandMask(ComDefCommandAngle, ComDefCommandMaskRet);
    ComDef_xu16GetPayloadLength(&txMessage) = sizeof (ComDefImu_TypeDef);
    ComDef_xu8GetCrc(&txMessage) = 0xFA;
    ComDef_xu8GetEnd(&txMessage) = 0xBA;
    ComDefImu_TypeDef * const pPayload = & ComDef_xpu8GetPayload(&txMessage);
    //txMessage.u16SlaveId = pDevice->UartSlaveInstance.u16SlaveId;
    //txMessage.u8Command = pDevice->UartSlaveInstance.u32CommandMode;
    //txMessage.u32Tick = HAL_GetTick();

    pPayload->fX = pDevice->pi16AccelXYZ[0];
    pPayload->fY = pDevice->pi16AccelXYZ[1];
    pPayload->fZ = pDevice->pi16AccelXYZ[2];
	pPayload->u32Tick = HAL_GetTick();
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
