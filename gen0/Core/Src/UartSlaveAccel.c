/**
 *
 *
 *
 */

#include "UartSlaveAccel.h"

#include "stm32f4_discovery.h"
#include "stm32f4_discovery_accelerometer.h"
#include "timerWrapper.h"

#include "main.h"
#include "comm.h"

extern TIM_HandleTypeDef htim1;


void UartSlaveAccel_StateMachine();

int32_t UartSlaveAccel_Init(UartSlaveAccel_TypeDef *pInit,
		UartSlaveIo_TypeDef *pIoContext, uint16_t SlaveId) {
	UartSlave_Init(&pInit->UartSlaveInstance, pIoContext, SlaveId);

	lowpass_Init(&pInit->LowpassAccelX, 0.02f);
	lowpass_Init(&pInit->LowpassAccelY, 0.02f);
	lowpass_Init(&pInit->LowpassAccelZ, 0.02f);
	pInit->pi16AccelXYZ[0] = 0;
	pInit->pi16AccelXYZ[1] = 0;
	pInit->pi16AccelXYZ[2] = 0;

	pInit->pi16AccelXYZOffset[0] = 0;
	pInit->pi16AccelXYZOffset[1] = 0;
	pInit->pi16AccelXYZOffset[2] = 0;

	pInit->UartSlaveInstance.pfvStateMachine = UartSlaveAccel_StateMachine;

	return 0;
}

//static ComDef_xpu8DeclareBuffer(txMessage, ComDefImu_TypeDef);

#define BUFFSIZE 1024
void UartSlaveAccel_StateMachine(void *pSlaveDevice) {
	UartSlaveAccel_TypeDef *pDevice = (UartSlaveAccel_TypeDef*) pSlaveDevice;
	//static uint8_t synced = 0;
	static uint8_t buffer[BUFFSIZE];
	static uint32_t bufferOffset = 0;
	static uint32_t imu_tick = 0;
	uint8_t  const * pBuffer = &buffer[bufferOffset];
	uint32_t offset = 0;


	// get Synced
	const uint32_t ringBufferLength = (pDevice->UartSlaveInstance.rxBufferManager.WriteIdx >= pDevice->UartSlaveInstance.rxBufferManager.ReadIdx)
                ? (pDevice->UartSlaveInstance.rxBufferManager.WriteIdx - pDevice->UartSlaveInstance.rxBufferManager.ReadIdx)
                : (pDevice->UartSlaveInstance.rxBufferManager.Length - pDevice->UartSlaveInstance.rxBufferManager.ReadIdx + pDevice->UartSlaveInstance.rxBufferManager.WriteIdx);

	// uint32_t RingBuffer_Read_XBit (RingBuffer_TypeDef * const pInit,
    // void * const pData,
    // size_t const Sizeof)

	RingBuffer_Read_XBit(&pDevice->UartSlaveInstance.rxBufferManager, (void *) &pBuffer [0], ringBufferLength);
	while (offset < ringBufferLength && *pBuffer != 0xAA)
	{
		pBuffer ++;
		offset ++;
	}
	if (offset >= ringBufferLength )
	{
		bufferOffset = offset;
		if (pDevice->UartSlaveInstance.u32CommandMode != ComDefModeStream)
			return;
	}
	bufferOffset = 0;

	//uint32_t calulatedLength = ComDef_xpu32CalculateLength(&buffer);
/*	if (abs(pDevice->UartSlaveInstance.rxBufferManager.ReadIdx - pDevice->UartSlaveInstance.rxBufferManager.WriteIdx) <= calulatedLength)
	{
		BufferOffset = abs(pBuffer - Buffer);
		return;
	}*/



	if (ComDef_xu8GetCrc(&buffer) != 0xFA)
	{
		//synced = 0;
		return;
	}

	//synced = 1;

	// state
	int8_t reProccess = 0;
	do {

		switch (ComDef_xu8GetCommand(&buffer) & ComDefCommandModeMask) {
		case (ComDefCommandMode):
			ComDef_xpu8DeclareBuffer(CommandModeBuffer, ComDefMode_TypeDef);

			ComDef_xu8GetHeading(&CommandModeBuffer) = 0xAA;
			ComDef_xu8GetCommand(&CommandModeBuffer) = ComDef_xu8CommandMask(
					ComDefCommandMode, ComDefCommandMaskRet);
			ComDef_xu16GetPayloadLength(&CommandModeBuffer) =
					(uint16_t) sizeof(ComDefMode_TypeDef);
			ComDefMode_TypeDef *pPayloadTx = (ComDefMode_TypeDef *)&ComDef_xpu8GetPayload(
					&CommandModeBuffer);

			ComDef_xu8GetCrc(&CommandModeBuffer) = 0xFA;
			ComDef_xu8GetEnd(&CommandModeBuffer) = 0xBB;

			ComDefMode_TypeDef *pPayloadRx = (ComDefMode_TypeDef *)&ComDef_xpu8GetPayload(&buffer);
			switch (ComDef_xu8GetCommand(&buffer) & ComDefCommandMask) {
			case (ComDefCommandMaskGet):
				pPayloadTx->u8Mode = pDevice->UartSlaveInstance.u32CommandMode;
				break;
			case (ComDefCommandMaskSet):
				pDevice->UartSlaveInstance.u32CommandMode = 0;
			pDevice->UartSlaveInstance.u32CommandMode = pPayloadRx->u8Mode;
				break;
			}

			pDevice->UartSlaveInstance.pIoContext->pfvTx( (uint8_t *)&CommandModeBuffer [0],
					sizeof(CommandModeBuffer));

			break;

/////////////////////////////////////////////////////////////////////////

		case (ComDefCommandImu):
			ComDef_xpu8DeclareBuffer(CommandImuBuffer, ComDefImu_TypeDef);

			switch (ComDef_xu8GetCommand(&buffer) & ComDefCommandMask) {
			case (ComDefCommandMaskGet):
				// fix reProccess
				if (1 >= reProccess)
				{
					reProccess = -1;
				}

				// get accels
				// BSP_ACCELERO_GetXYZ(&pDevice->pi16AccelXYZ[0]);
				BSP_ACCELERO_GetXYZ(&pDevice->pi16AccelXYZ[0]);
				// update the lowpasses
				lowpass_update(&pDevice->LowpassAccelX,
						pDevice->pi16AccelXYZ[0] - pDevice->pi16AccelXYZOffset[0]);
				lowpass_update(&pDevice->LowpassAccelY,
						pDevice->pi16AccelXYZ[1] - pDevice->pi16AccelXYZOffset[1]);
				lowpass_update(&pDevice->LowpassAccelZ,
						pDevice->pi16AccelXYZ[2] - pDevice->pi16AccelXYZOffset[2]);

				//    static UartSlaveMessage_TypeDef txMessage;
				ComDef_xu8GetHeading(&CommandImuBuffer) = 0xAA;
				ComDef_xu8GetCommand(&CommandImuBuffer) = ComDef_xu8CommandMask(
						ComDefCommandImu, ComDefCommandMaskRet);
				ComDef_xu16GetPayloadLength(&CommandImuBuffer) =
						sizeof(ComDefImu_TypeDef);
				ComDef_xu8GetCrc(&CommandImuBuffer) = 0xFA;
				ComDef_xu8GetEnd(&CommandImuBuffer) = 0xBA;
				ComDefImu_TypeDef *const pPayload = (ComDefImu_TypeDef *const )&ComDef_xpu8GetPayload(
						&CommandImuBuffer);
				//txMessage.u16SlaveId = pDevice->UartSlaveInstance.u16SlaveId;
				//txMessage.u8Command = pDevice->UartSlaveInstance.u32CommandMode;
				//txMessage.u32Tick = HAL_GetTick();

				pPayload->Tick = imu_tick ++;
				pPayload->Encoder = ACCELDEVICE_ENCODER;

				pPayload->fX = (float)pDevice->pi16AccelXYZ[0] - (float)pDevice->pi16AccelXYZOffset[0];
				pPayload->fY = (float)pDevice->pi16AccelXYZ[1] - (float)pDevice->pi16AccelXYZOffset[1];
				pPayload->fZ = (float)pDevice->pi16AccelXYZ[2] - (float)pDevice->pi16AccelXYZOffset[2];

				pDevice->UartSlaveInstance.pIoContext->pfvTx((uint8_t *)&CommandImuBuffer [0],
						sizeof(CommandImuBuffer));
				break;
//
//			case (COmDefCommandMaskImuSpecificOffset):
//				BSP_ACCELERO_GetXYZ(&pDevice->pi16AccelXYZOffset[0]);
//
//				break;

			}

			break;

/////////////////////////////////////////////////////////////////////////

		case (ComDefCommandSamplingRate):
			ComDef_xpu8DeclareBuffer(CommandSamplingRateBuffer, ComDefSamplingRate_TypeDef);

			ComDef_xu8GetHeading(&CommandSamplingRateBuffer) = 0xAA;
			ComDef_xu8GetCommand(&CommandSamplingRateBuffer) = ComDef_xu8CommandMask(
					ComDefCommandSamplingRate, ComDefCommandMaskRet);
			ComDef_xu16GetPayloadLength(&CommandSamplingRateBuffer) =
					sizeof(ComDefSamplingRate_TypeDef);
			ComDef_xu8GetCrc(&CommandSamplingRateBuffer) = 0xFA;
			ComDef_xu8GetEnd(&CommandSamplingRateBuffer) = 0xBA;
			ComDefSamplingRate_TypeDef * pPayloadTx_ = (ComDefSamplingRate_TypeDef * )&ComDef_xpu8GetPayload(
					&CommandSamplingRateBuffer);
			ComDefSamplingRate_TypeDef *pPayloadRx_ = (ComDefSamplingRate_TypeDef *)&ComDef_xpu8GetPayload(&buffer);


			switch (ComDef_xu8GetCommand(&buffer) & ComDefCommandMask) {
			case (ComDefCommandMaskGet):
				{
					uint32_t payingIdeaOfSettingSamplingRateAs16Bit = 0;
					xu32TimerWrapper_getFreq (ACCELDEVICE_INSTANCE, &payingIdeaOfSettingSamplingRateAs16Bit);
					pPayloadTx_->u16SamplingRate = (uint16_t)payingIdeaOfSettingSamplingRateAs16Bit;
				}
				break;
			case (ComDefCommandMaskSet):
				{
					uint32_t payingIdeaOfSettingSamplingRateAs16Bit = pPayloadRx_->u16SamplingRate;
					xvTimerWrapper_SetFreq (ACCELDEVICE_INSTANCE, payingIdeaOfSettingSamplingRateAs16Bit);
					xu32TimerWrapper_getFreq (ACCELDEVICE_INSTANCE, &payingIdeaOfSettingSamplingRateAs16Bit);

					pPayloadTx_->u16SamplingRate = (uint16_t)payingIdeaOfSettingSamplingRateAs16Bit;
				}
				break;
			case (ComDefCommandMaskRet):
				break;
			}
			pDevice->UartSlaveInstance.pIoContext->pfvTx((uint8_t *)&CommandSamplingRateBuffer [0],
					sizeof(CommandSamplingRateBuffer));
			break;

/////////////////////////////////////////////////////////////////////////

		case (ComDefCommandAngle):
			switch (ComDef_xu8GetCommand(&buffer) & ComDefCommandMask) {
			case (ComDefCommandMaskGet):
				break;
			case (ComDefCommandMaskSet):
				break;
			case (ComDefCommandMaskRet):
				break;
			}
			break;

		}

		switch (pDevice->UartSlaveInstance.u32CommandMode) {
		case ComDefModeStream:
			if (0 <= reProccess)
			{
				ComDef_xu8GetCommand(&buffer) = ComDef_xu8CommandMask(ComDefCommandImu, ComDefCommandMaskGet);
				reProccess = 1;
			}

			break;
		case ComDefModeOnRequest:
			///@ the codeblock already does the onrequest part
			break;

		case ComDefModeStop:
		default:
			break;
		}

	} while (reProccess > 0);
}
