/*
 * @file ring_buffer.h
 * @brief This file has defines, and typedefs to provide user a ring buffer
 */

#ifndef INC_RING_BUFFER_H_
#define INC_RING_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif
/* Includes */
#include <stdint.h>
#include "stddef.h"
/* defines */
#define RING_BUFFER_OK (0)

#define RING_BUFFER_ERR_LENGTH_LESSORREQUALTOZERO		(-1)
#define RING_BUFFER_ERR_READ_IDX_LESSOREQALTOZERO		(-2)
#define RING_BUFFER_ERR_WRITE_IDX_LESSOREQALTOZERO	(-3)

/* TypeDefs */
/**
 * @brief This is a struct of ring buffer. User should not edit or put infomation by hand, but use Init function
 * @note This implementation can used with different size variables, but user should be aware of the order of the data in the buffer
 */
typedef struct RingBuffer_s
{
	/* User */
	uint32_t Length; /**< Length of Buffer */
	void * pBuffer; /**< Buffer that user provided */
	/* Control */
	uint32_t WriteIdx; /**< Write Index of Buffer */
	uint32_t ReadIdx; /**< Read Index of Buffer */

} RingBuffer_TypeDef;

int32_t RingBuffer_Init(RingBuffer_TypeDef * const pInit,
                     void * const pBuffer,
                     uint32_t const Length);

int32_t RingBuffer_DeInit(RingBuffer_TypeDef * pInit);

uint32_t RingBuffer_Write_XBit (RingBuffer_TypeDef * const pInit,
                            void * const pData,
                           size_t const Sizeof);

uint32_t RingBuffer_Read_XBit (RingBuffer_TypeDef * const pInit,
                           void * const pData,
                          size_t const Sizeof);

#ifdef __cplusplus
}
#endif

#endif /* INC_RING_BUFFER_H_ */
