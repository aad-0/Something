/**
 * @file uartSlave.h
 * @brief Interface for any UartSlave
 *
 */

#ifndef UARTSLAVE_H
#define UARTSLAVE_H

/* Includes */
#include "stdint.h"
#include "ring_buffer.h"

#define RXBUFFERSIZE (2048U)


#define xu8UartSlaveGetPayload(__POINTER__, __IDX)	\
	( (uint8_t *) &((__POINTER__))->u16PayloadLength +1 + ( ((__IDX)) * sizeof(uint8_t)   ) )

#define xu16UartSlaveGetPayload(__POINTER__, __IDX)	\
	( (uint8_t *) &((__POINTER__))->u16PayloadLength +1 + ( ((__IDX )) * sizeof(uint16_t) ) )

#define xu32UartSlaveGetPayload(__POINTER__, __IDX)	\
	( (uint8_t *) &((__POINTER__))->u16PayloadLength +1 + ( ((__IDX )) * sizeof(uint32_t) ) )


#define xvUartSlaveSetTxFlag(__POINTER__, __FLAG)	\
	( ((__POINTER__))->u32TxFlags = ((__POINTER__))->u32TxFlags |  ((__FLAG )) )

#define xvUartSlaveClearTxFlag(__POINTER__, __FLAG)	\
	( ((__POINTER__))->u32TxFlags = ((__POINTER__))->u32TxFlags & ~((__FLAG )) )


#define xvUartSlaveSetRxFlag(__POINTER__, __FLAG)	\
	( ((__POINTER__))->u32RxFlags = ((__POINTER__))->u32RxFlags |  ((__FLAG )) )

#define xvUartSlaveClearRxFlag(__POINTER__, __FLAG)	\
	( ((__POINTER__))->u32RxFlags = ((__POINTER__))->u32RxFlags & ~((__FLAG )) )




/* TypeDefs */

/**
 *
 */
typedef struct uart_slave_message_heading_s
{
	uint8_t u8Heading;
	uint16_t u16SlaveId;
	uint8_t u8Command;
	uint16_t u16PayloadLength;
	// Payload
	// CRC
	// endChar
} UartSlaveMessageHeading_TypeDef;

typedef struct uart_slave_io_s
{
	void ( * pfvCallbackRx ) ();
	void ( * pfvCallbackTx ) ();

	void ( * pfvTx ) (void * pBuffer, size_t BufferSize);
	void ( * pfvRx ) (void * pBuffer, size_t BufferSize);

} UartSlaveIo_TypeDef;

typedef enum uart_slave_commands_e
{
	UARTSLAVE_COMMAND_MODE_NONE			= 0x00U,
	UARTSLAVE_COMMAND_MODE_ONREQUEST 	= 0x01U,
	UARTSLAVE_COMMAND_MODE_STREAM 		= 0x02U,
} UartSlaveCommands_TypeDef;

typedef struct uart_slave_s
{
	uint16_t u16SlaveId;
	uint32_t u32CommandMode;

	uint32_t u32TxFlags;
	uint32_t u32RxFlags;

	UartSlaveIo_TypeDef * pIoContext;

	RingBuffer_TypeDef rxBufferManager; // Bunu soyutlamadim cunku sikerler
	uint8_t pu8RxBufferRaw [RXBUFFERSIZE];

	uint8_t pu8TxBufferRaw [RXBUFFERSIZE];

	void ( * pfvStateMachine ) (void * pSlaveDevice);
} UartSlave_TypeDef;

/*typedef enum uart_slave_commands_e
{
	//setId = 1,
} UartSlaveCommands_TypeDef;*/

int32_t UartSlave_Init (UartSlave_TypeDef * pInit, UartSlaveIo_TypeDef * pIoContext, uint16_t SlaveId);
uint32_t UartSlave_setCommandMode (UartSlave_TypeDef * pInit, UartSlaveCommands_TypeDef CommandMode);

#endif //UARTSLAVE_H
