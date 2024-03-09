
#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "BLLibrarys.h"
#ifdef __cplusplus
	};
#endif
/* TODO: Add your comment here */
signed short Unscale(float value, float high_range, float low_range, signed short analog_high, signed short analog_low)
{
	/*TODO: Add your code here*/
	int temp;
	if(high_range == low_range)
	{
		return analog_low;
	}
		
	if (value < low_range)
	{
		return analog_low;
	}
	
	if (value > high_range)
	{
		return analog_high;
	}
	
	temp = (signed short)(value - low_range) * (analog_high - analog_low) / (signed short)(high_range - low_range) + analog_low;
	return temp;
}
