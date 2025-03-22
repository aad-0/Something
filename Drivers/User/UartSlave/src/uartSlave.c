/**
 *
 *
 */

#include "uartSlave.h"

int32_t UartSlave_Init (UartSlave_TypeDef * pInit, UartSlaveIo_TypeDef * pIoContext, uint16_t SlaveId)
{
	RingBuffer_Init(& pInit->rxBufferManager, & pInit->pu8RxBufferRaw[0], RXBUFFERSIZE);
	pInit->u16SlaveId = SlaveId;
	pInit->u32CommandMode = UARTSLAVE_COMMAND_MODE_STREAM; //UARTSLAVE_COMMAND_MODE_NONE;
	pInit->pIoContext = pIoContext;

	return 0;
}

uint32_t UartSlave_setCommandMode (UartSlave_TypeDef * pInit, UartSlaveCommands_TypeDef CommandMode)
{
	switch (CommandMode)
	{
	case (UARTSLAVE_COMMAND_MODE_ONREQUEST):
	case (UARTSLAVE_COMMAND_MODE_STREAM):
		pInit->u32CommandMode = CommandMode;
	default:
		pInit->u32CommandMode = UARTSLAVE_COMMAND_MODE_NONE;
	}

	return (uint32_t)pInit->u32CommandMode;

}
