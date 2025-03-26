/*
 * @file: ring_buffer.c
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "ring_buffer.h"


/**
 * @brief This is a init function for ring buffer
 * @param pInit Instance of Ring Buffer
 * @param pBuffer User's Buffer
 * @param Length Length of Buffer
 * @note User should not write to buffer manually
 * @retval Status Code
 */
int32_t RingBuffer_Init(RingBuffer_TypeDef * const pInit,
                     void * const pBuffer,
                     uint32_t const Length)
{
	pInit->Length = Length;
	pInit->pBuffer = pBuffer;

	pInit->WriteIdx = 0;
	pInit->ReadIdx = 0;

	return 0;
}

/**
 * @brief This function deInitializes a ring buffer
 * @param pInit Instance of ring buffer
 * @retval Status Code
 */
int32_t RingBuffer_DeInit(RingBuffer_TypeDef * pInit)
{
  (void) (pInit);
	return 0;
}

/**
 * @brief This Function writes to ring buffer
 * @param pInit Instance of Ring Buffer
 * @param pData Pointer of data
 * @param Sizeof Sizeof Data to write
 * @retval Where is the WriteIDX is
 */
uint32_t RingBuffer_Write_XBit (RingBuffer_TypeDef * const pInit,
                            void * const pData,
                           size_t const Sizeof)
{
	uint32_t i;
	for (i = 0; i < Sizeof; ++i)
		( (uint8_t *)(pInit->pBuffer) )[(pInit->WriteIdx +i) % (pInit->Length)] =
				( (uint8_t *)(pData) )[i];

  pInit->WriteIdx = (pInit->WriteIdx +Sizeof) % (pInit->Length);
	return pInit->WriteIdx;
}

/**
 * @brief This Function reads from ring buffer
 * @param pInit Instance of Ring Buffer
 * @param pData Pointer of where to write
 * @param Sizeof Sizeof where to write and how much to read
 * @retval Where is the ReadIDX is
 */
uint32_t RingBuffer_Read_XBit (RingBuffer_TypeDef * const pInit,
                           void * const pData,
                          size_t const Sizeof)
{
	uint32_t i;
	for (i = 0; i < Sizeof; ++i)
				( (uint8_t *)(pData) ) [i] =
						( (uint8_t *)(pInit->pBuffer) )[(pInit->ReadIdx +i) % (pInit->Length)];

  pInit->ReadIdx = (pInit->ReadIdx +Sizeof) % (pInit->Length);
	return pInit->ReadIdx;
}

#ifdef __cplusplus
}
#endif
