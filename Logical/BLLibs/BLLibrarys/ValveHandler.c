
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
plcbit ValveHandler(struct Valve* valve)
{
	/*TODO: Add your code here*/
	
	/* Mode select*/
	valve->status.mode = valve->cmd.mode;
	
	/*Out of service mode*/
	if (valve->status.mode == 3)
	{
		valve->status.open_fault = 0;
		valve->status.close_fault = 0;
		valve->status.emergency_stop = 0;
		valve->status.interlock = 0;
		valve->status.mutex = 0;
		valve->status.open = 0;
		valve->status.state = 0;
		return 0;
	}
	
	/*Local mode*/
	if(valve->status.mode == 2)
	{
		valve->status.open = valve->io.open_feedback;
		valve->status.emergency_stop = valve->io.emergency_stop | valve->input.local_stop;
		valve->status.interlock = 0;
		valve->status.mutex = 0;
		valve->status.close_fault = 0;
		valve->status.open_fault = 0;
	}
	
	/*Automatic mode*/
	
	
	/*Manual mode*/
}
