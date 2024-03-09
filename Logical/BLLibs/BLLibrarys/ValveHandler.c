
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
	if (valve->status.mode == OUT_SERVICE_MODE)
	{
		valve->status.open_fault = 0;
		valve->status.close_fault = 0;
		valve->status.emergency_stop = 0;
		valve->status.interlock = 0;
		valve->status.mutex = 0;
		valve->status.open = 0;
		valve->status.open_feedback = 0;
		valve->status.close_feedback = 0;
		valve->status.state = 0;
		return 0;
	}
	
	/*Local mode*/
	if(valve->status.mode == LOCAL_MODE)
	{
		valve->status.open = valve->io.open_feedback;
		valve->status.open_feedback = valve->io.open_feedback;
		valve->status.close_feedback = valve->io.close_feedback;
		valve->status.emergency_stop = valve->io.emergency_stop;
		valve->status.interlock = 0;
		valve->status.mutex = 0;
		valve->status.open_fault = 0;
		valve->status.close_fault = 0;
		valve->status.state = valve->status.open | (valve->status.open_feedback << 1) | (valve->status.close_feedback << 2) | (valve->status.emergency_stop << 3) | 
			(valve->status.interlock << 4) | (valve->status.mutex << 5) | (valve->status.open_fault << 6) | (valve->status.close_fault << 7);
		return 1;
	}
	
	/**/
	if (valve->para.open_feedback_enable && valve->para.close_feedback_enable)
	{
		valve->input.open_feedback_state = valve->status.open && valve->io.open_feedback && !valve->io.close_feedback;
		valve->input.close_feedback_state = !valve->status.open && !valve->io.open_feedback && valve->io.close_feedback;
	}
	else if(valve->para.open_feedback_enable && !valve->para.close_feedback_enable)
	{
		valve->input.open_feedback_state = valve->status.open && valve->io.open_feedback;
		valve->input.close_feedback_state = !valve->status.open && !valve->io.open_feedback;	
	}
	else if (!valve->para.open_feedback_enable && valve->para.close_feedback_enable)
	{
		valve->input.open_feedback_state = valve->status.open && !valve->io.close_feedback;
		valve->input.close_feedback_state = !valve->status.open && valve->io.close_feedback;
	}
	else
	{
		valve->input.open_feedback_state = valve->status.open;
		valve->input.close_feedback_state = !valve->status.open;
	}
	
	/*Emergency stop*/
	if (valve->io.emergency_stop | valve->input.stop_signal)
	{
		valve->status.emergency_stop = 1;
	}
	else
	{
		if(valve->cmd.reset)
		{
			valve->status.emergency_stop = 0;
		}
	}
	
	/*Interlock*/
	if (valve->input.interlock_signal)
	{
		valve->status.interlock = 1;
	}
	else
	{
		if(valve->cmd.reset)
		{
			valve->status.interlock = 0;
		}
	}
	
	/*Mutex*/
	valve->status.mutex = valve->input.mutex_signal && !valve->cmd.mutex_disable;
	
	/*Feedback state*/
	valve->status.open_feedback = valve->io.open_feedback;
	valve->status.close_feedback = valve->io.close_feedback;
	
	/*Feedback fault*/
	valve->status.open_fault = OutputDelay((valve->status.open && !valve->input.open_feedback_state), &valve->process.open_fault_timer, valve->para.open_fault_delay);
	valve->status.close_fault = OutputDelay((!valve->status.open && !valve->input.close_feedback_state), &valve->process.close_fault_timer, valve->para.close_fault_delay);
	
	/*Automatic mode*/
	if(valve->status.mode == AUTOMATIC_MODE)
	{
		if (valve->para.is_interlock_open)
		{
			valve->status.open = valve->status.interlock || (valve->input.auto_open_signal && !valve->status.emergency_stop && !valve->status.mutex);
		}
		else
		{
			valve->status.open = valve->input.auto_open_signal && !valve->status.emergency_stop && !valve->status.interlock && !valve->status.mutex;
		}
	}
	
	/*Manual mode*/
	if (valve->status.mode == MANUAL_MODE)
	{
		if (valve->para.is_interlock_open)
		{
			plcbit manual_open;
			
			if (valve->cmd.open && !valve->status.emergency_stop && !valve->status.mutex)
			{
				manual_open = 1;
			}
			if (valve->cmd.close || valve->status.emergency_stop || valve->status.mutex)
			{
				manual_open = 0;
			}
			valve->status.open = valve->status.interlock || manual_open;
		}
		else
		{
			if (valve->cmd.open && !valve->status.emergency_stop && !valve->status.interlock && !valve->status.mutex)
			{
				valve->status.open = 1;
			}
			if (valve->cmd.close || valve->status.emergency_stop || valve->status.interlock || valve->status.mutex)
			{
				valve->status.open = 0;
			}
		}
	}

	/*Output*/
	valve->io.valve_open = valve->status.open;
	
	/*Reset command*/
	if (valve->cmd.open)
	{
		valve->cmd.open = 0;
	}
	if (valve->cmd.close)
	{
		valve->cmd.close = 0;
	}
	if (valve->cmd.reset)
	{
		valve->cmd.reset = 0;
	}
	
	return 1;
}
