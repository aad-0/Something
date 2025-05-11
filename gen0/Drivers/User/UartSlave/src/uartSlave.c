/**
 *
 *
 */

#include "uartSlave.h"

int32_t UartSlave_Init (UartSlave_TypeDef * pInit, UartSlaveIo_TypeDef * pIoContext, uint16_t SlaveId)
{
	RingBuffer_Init(& pInit->rxBufferManager, & pInit->pu8RxBufferRaw[0], RXBUFFERSIZE);
	pInit->u16SlaveId = SlaveId;
	pInit->u32CommandMode = 0; //UARTSLAVE_COMMAND_MODE_NONE;
	pInit->pIoContext = pIoContext;

	return 0;
}

