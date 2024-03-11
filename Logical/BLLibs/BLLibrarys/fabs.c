
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
float fabs(float value)
{
	/*TODO: Add your code here*/
	if (value < 0.0f)
	{
		return -(value);
	}
	else
	{
		return (value);
	}
}
