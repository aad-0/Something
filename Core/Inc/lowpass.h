#ifndef LOWPASS_H
#define LOWPASS_H

#ifdef __cplusplus
extern "C"
{
#endif


#include "stdint.h"
typedef struct lowpass_s
{
	float alpha;
	float out;
} Lowpass_TypeDef;

int32_t lowpass_Init (Lowpass_TypeDef * pInit, float Alpha);
float lowpass_update (Lowpass_TypeDef * pInit, float New);



#ifdef __cplusplus
}
#endif

#endif
