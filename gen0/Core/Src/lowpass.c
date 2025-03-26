#include "lowpass.h"

int32_t lowpass_Init (Lowpass_TypeDef * pInit, float Alpha)
{
	pInit->alpha = Alpha;
	pInit->out = 0;

	return 0;
}

float lowpass_update (Lowpass_TypeDef * pInit, float New)
{
	pInit->out = pInit->alpha *New + (1 - pInit->alpha) * pInit->out;
	return pInit->out;
}
