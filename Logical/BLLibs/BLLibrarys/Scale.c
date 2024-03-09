
#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "BLLibrarys.h"
#ifdef __cplusplus
	};
#endif
/* TODO:min-max normalization */
float Scale(signed short analog, float high_range, float low_range, signed short analog_high, signed short analog_low)
{
	/*TODO: Add your code here*/		
	if (analog < analog_low)
	{
		return low_range;
	}
	
	if (analog > analog_high)
	{
		return high_range;
	}
	
	return ((float)(analog - analog_low) * (high_range - low_range) / (float)(analog_high - analog_low) + low_range);

}
