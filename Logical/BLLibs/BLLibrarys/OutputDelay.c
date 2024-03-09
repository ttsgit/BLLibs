
#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "BLLibrarys.h"
#ifdef __cplusplus
	};
#endif
/* TODO: Delay seconds to output */
plcbit OutputDelay(plcbit input, plctime* timer, signed long second)
{
	/*TODO: Add your code here*/
	plcbit output = 0;
	if(input)
	{
		if((clock_ms() - *timer) >= (second * 1000))
		{
			output = 1;
		}
	}
	else
	{
		output = 0;
		*timer = clock_ms();
	}
	return output;
}
