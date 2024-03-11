
#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
#include "BLLibrarys.h"
#ifdef __cplusplus
	};
#endif


#define FAULT_BITS 0xFFF0


void reset_cmd(struct ValveCmd* cmd);


/* TODO: Add your comment here */
plcbit ValveHandler(struct Valve* valve)
{
	/*TODO: Add your code here*/
	/* Mode select*/
	valve->status.mode = valve->cmd.mode;
	
	/*Out of service mode*/
	if (valve->status.mode == OUT_SERVICE_MODE)
	{
		valve->internal.mode_edge = 0;
		valve->status.open_fault = 0;
		valve->status.close_fault = 0;
		valve->status.emergency_stop = 0;
		valve->status.interlock = 0;
		valve->status.mutex = 0;
		valve->status.open = 0;
		valve->status.open_feedback = 0;
		valve->status.close_feedback = 0;
		valve->status.state = 0;
		reset_cmd(&(valve->cmd));
		return 0;
	}
	
	/*Local mode*/
	if(valve->status.mode == LOCAL_MODE)
	{
		valve->internal.mode_edge = 0;
		valve->status.open = valve->io.open_feedback;
		valve->status.open_feedback = valve->io.open_feedback;
		valve->status.close_feedback = valve->io.close_feedback;
		valve->status.emergency_stop = valve->io.emergency_stop;
		valve->status.interlock = 0;
		valve->status.mutex = 0;
		valve->status.open_fault = 0;
		valve->status.close_fault = 0;
		valve->status.state = valve->status.open | (valve->status.open_feedback << 1) | (valve->status.close_feedback << 2) | (valve->status.mutex << 3) | 
			(valve->status.open_fault << 4) | (valve->status.close_fault << 5) | (valve->status.interlock << 6) | (valve->status.emergency_stop << 7);
		reset_cmd(&(valve->cmd));
		return 1;
	}
	
	/*Simulation*/
	if (valve->cmd.simulation)
	{
		valve->process.open_feedback_state = valve->status.open;
		valve->process.close_feedback_state = !valve->status.open;
	}
	else
	{
		if (valve->cmd.open_feedback_enable && valve->cmd.close_feedback_enable)
		{
			valve->process.open_feedback_state = valve->status.open && valve->io.open_feedback && !valve->io.close_feedback;
			valve->process.close_feedback_state = !valve->status.open && !valve->io.open_feedback && valve->io.close_feedback;
		}
		else if(valve->cmd.open_feedback_enable && !valve->cmd.close_feedback_enable)
		{
			valve->process.open_feedback_state = valve->status.open && valve->io.open_feedback;
			valve->process.close_feedback_state = !valve->status.open && !valve->io.open_feedback;
		}
		else if (!valve->cmd.open_feedback_enable && valve->cmd.close_feedback_enable)
		{
			valve->process.open_feedback_state = valve->status.open && !valve->io.close_feedback;
			valve->process.close_feedback_state = !valve->status.open && valve->io.close_feedback;
		}
		else
		{
			valve->process.open_feedback_state = valve->status.open;
			valve->process.close_feedback_state = !valve->status.open;
		}
	}

	
	/*Emergency stop*/
	if (valve->io.emergency_stop | valve->process.stop_signal)
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
	if (valve->cmd.interlock_enable)
	{
		if (valve->process.interlock_signal)
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
	}
	else
	{
		valve->status.interlock = 0;
	}

	/*Mutex*/
	valve->status.mutex = valve->process.mutex_signal && valve->cmd.mutex_enable;

	/*Feedback state*/
	valve->status.open_feedback = valve->io.open_feedback;
	valve->status.close_feedback = valve->io.close_feedback;
	
	/*Feedback fault*/
	valve->status.open_fault = OutputDelay((valve->status.open && !valve->process.open_feedback_state), &valve->internal.open_fault_timer, valve->cmd.open_fault_delay);
	valve->status.close_fault = OutputDelay((!valve->status.open && !valve->process.close_feedback_state), &valve->internal.close_fault_timer, valve->cmd.close_fault_delay);
	
	/*Automatic mode*/
	if(valve->status.mode == AUTOMATIC_MODE)
	{
		valve->internal.mode_edge = 0;
		if (valve->cmd.is_interlock_open)
		{
			valve->status.open = valve->status.interlock || (valve->process.auto_open_signal && !valve->status.emergency_stop && !valve->status.mutex);
		}
		else
		{
			valve->status.open = valve->process.auto_open_signal && !valve->status.emergency_stop && !valve->status.interlock && !valve->status.mutex;
		}
	}
	
	/*Manual mode*/
	if (valve->status.mode == MANUAL_MODE && !valve->internal.mode_edge)
	{
		/*Reset open state, when the mode switch from others to manual*/
		valve->status.open = 0;
		valve->internal.mode_edge = 1;
	}
	if (valve->status.mode == MANUAL_MODE)
	{
		if (valve->cmd.is_interlock_open)
		{
			if (valve->cmd.open && !valve->status.emergency_stop && !valve->status.mutex)
			{
				valve->internal.manual_open = 1;
			}
			if (valve->cmd.close || valve->status.emergency_stop || valve->status.mutex)
			{
				valve->internal.manual_open = 0;
			}
			valve->status.open = valve->status.interlock || valve->internal.manual_open;
		}
		else
		{
			if (valve->cmd.open && !valve->status.emergency_stop && !valve->status.interlock && !valve->status.mutex)
			{
				valve->internal.manual_open = 1;
			}
			if (valve->cmd.close || valve->status.emergency_stop || valve->status.interlock || valve->status.mutex)
			{
				valve->internal.manual_open = 0;
			}
			valve->status.open = valve->internal.manual_open;
		}
	}


	valve->status.state = valve->status.open | (valve->status.open_feedback << 1) | (valve->status.close_feedback << 2) | (valve->status.mutex << 3) | 
		(valve->status.open_fault << 4) | (valve->status.close_fault << 5) | (valve->status.interlock << 6) | (valve->status.emergency_stop << 7);
	
	valve->process.fault = (valve->status.state & FAULT_BITS)?1:0;
	
	/*Reset command*/
	reset_cmd(&(valve->cmd));
	
	/*Output*/
	if (valve->cmd.simulation)
	{
		valve->io.valve_open = 0;
	}
	else
	{
		valve->io.valve_open = valve->status.open;
	}
	
	return 1;
}

/*Reset command*/
void reset_cmd(struct ValveCmd* cmd)
{
	if (cmd->open)
	{
		cmd->open = 0;
	}
	if (cmd->close)
	{
		cmd->close = 0;
	}
	if (cmd->reset)
	{
		cmd->reset = 0;
	}
}
