
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

static void reset_cmd(struct InvertMotorCmd* cmd);


/* TODO: Add your comment here */
plcbit InvertMotorHandler(struct InvertMotor* motor)
{
	/*TODO: Add your code here*/
	/* Mode select*/
	motor->status.mode = motor->cmd.mode;
	
	/*Out of service mode*/
	if (motor->status.mode == OUT_SERVICE_MODE)
	{
		motor->internal.mode_edge = 0;
		motor->status.fault = 0;
		motor->status.emergency_stop = 0;
		motor->status.interlock = 0;
		motor->status.mutex = 0;
		motor->status.run = 0;
		motor->status.run_feedback = 0;
		motor->status.fault_feedback = 0;
		motor->status.state = 0;
		motor->status.current_feedback = 0;
		motor->status.speed_feedback = 0;
		motor->status.speed = 0;
		reset_cmd(&(motor->cmd));
		return 0;
	}
	
	/*Local mode*/
	if(motor->status.mode == LOCAL_MODE)
	{
		motor->internal.mode_edge = 0;
		motor->status.run = motor->io.run_feedback;
		motor->status.run_feedback = motor->io.run_feedback;
		motor->status.fault_feedback = motor->io.fault_feedback;
		motor->status.emergency_stop = motor->io.emergency_stop;
		motor->status.interlock = 0;
		motor->status.mutex = 0;
		motor->status.fault = motor->io.fault_feedback;
		motor->status.current_feedback = motor->process.current_feedback;
		motor->status.speed_feedback = motor->process.speed_feedback;
		motor->status.speed = 0;
		motor->status.state = motor->status.run | (motor->status.run_feedback << 1) | (motor->status.fault_feedback << 2) | (motor->status.mutex << 3) | 
			(motor->status.fault << 4) | (motor->status.interlock << 5) | (motor->status.emergency_stop << 6);
		reset_cmd(&(motor->cmd));
		return 1;
	}
	
	/*Simulation*/
	if (motor->cmd.simulation)
	{
		motor->process.run_feedback_state = motor->status.run;
		motor->process.stop_feedback_state = !motor->status.run;
	}
	else
	{
		if (motor->cmd.run_feedback_enable && motor->cmd.fault_feedback_enable)
		{
			motor->process.run_feedback_state = motor->status.run && motor->io.run_feedback && !motor->io.fault_feedback;
			motor->process.stop_feedback_state = !motor->status.run && !motor->io.run_feedback && !motor->io.fault_feedback;
		}
		else if(motor->cmd.run_feedback_enable && !motor->cmd.fault_feedback_enable)
		{
			motor->process.run_feedback_state = motor->status.run && motor->io.run_feedback;
			motor->process.stop_feedback_state = !motor->status.run && !motor->io.run_feedback;

		}
		else if (!motor->cmd.run_feedback_enable && motor->cmd.fault_feedback_enable)
		{
			motor->process.run_feedback_state = motor->status.run && !motor->io.fault_feedback;
			motor->process.stop_feedback_state = !motor->status.run && !motor->io.fault_feedback;
		}
		else
		{
			motor->process.run_feedback_state = motor->status.run;
			motor->process.stop_feedback_state = !motor->status.run;
		}
	}

	
	/*Emergency stop*/
	if (motor->io.emergency_stop | motor->process.stop_signal)
	{
		motor->status.emergency_stop = 1;
	}
	else
	{
		if(motor->cmd.reset)
		{
			motor->status.emergency_stop = 0;
		}
	}
	
	/*Interlock*/
	if (motor->cmd.interlock_enable)
	{
		if (motor->process.interlock_signal)
		{
			motor->status.interlock = 1;
		}
		else
		{
			if(motor->cmd.reset)
			{
				motor->status.interlock = 0;
			}
		}
	}
	else
	{
		motor->status.interlock = 0;
	}

	/*Mutex*/
	motor->status.mutex = motor->process.mutex_signal && motor->cmd.mutex_enable;

	/*Feedback state*/
	motor->status.current_feedback = motor->process.current_feedback;
	motor->status.speed_feedback = motor->process.speed_feedback;
	motor->status.run_feedback = motor->io.run_feedback;
	motor->status.fault_feedback = motor->io.fault_feedback;
	
	/*Feedback fault*/
	if (OutputDelay((motor->status.run && !motor->process.run_feedback_state) || (!motor->status.run && !motor->process.stop_feedback_state), &motor->internal.run_fault_timer, motor->cmd.fault_delay))
	{
		motor->status.fault = 1;
	}
	else
	{
		if(motor->cmd.reset)
		{
			motor->status.fault = 0;
		}
	}
	
	/*Automatic mode*/
	if(motor->status.mode == AUTOMATIC_MODE)
	{
		motor->internal.mode_edge = 0;
		
		motor->status.speed = motor->process.auto_speed;
		
		if (motor->cmd.is_interlock_open)
		{
			motor->status.run = motor->status.interlock || (motor->process.auto_run_signal && !motor->status.fault && !motor->status.emergency_stop && !motor->status.mutex);
		}
		else
		{
			motor->status.run = motor->process.auto_run_signal && !motor->status.fault && !motor->status.emergency_stop && !motor->status.interlock && !motor->status.mutex;
		}
	}
	
	/*Manual mode*/
	if (motor->status.mode == MANUAL_MODE && !motor->internal.mode_edge)
	{
		/*Reset open state, when the mode switch from others to manual*/
		motor->status.run = 0;
		motor->internal.mode_edge = 1;
	}
	if (motor->status.mode == MANUAL_MODE)
	{
		motor->status.speed = motor->cmd.speed;
		
		if (motor->cmd.jog && !motor->status.fault && !motor->status.emergency_stop && !motor->status.interlock && !motor->status.mutex)
		{
			motor->internal.jog_run = 1;
		}
		if(motor->internal.jog_run)
		{
			if((clock_ms() - motor->internal.jog_timer) >= (motor->cmd.jog_delay * 1000))
			{
				motor->internal.jog_run = 0;
			}
		}
		else
		{
			motor->internal.jog_timer = clock_ms();	
		}
		
		if (motor->cmd.is_interlock_open)
		{
			if (motor->cmd.start && !motor->status.fault && !motor->status.emergency_stop && !motor->status.mutex)
			{
				motor->internal.manual_run = 1;
			}
			if (motor->cmd.stop || motor->status.fault || motor->status.emergency_stop || motor->status.mutex)
			{
				motor->internal.manual_run = 0;
			}
			motor->status.run = motor->status.interlock || motor->status.fault || motor->internal.manual_run || motor->internal.jog_run;
		}
		else
		{
			if (motor->cmd.start && !motor->status.fault && !motor->status.emergency_stop && !motor->status.interlock && !motor->status.mutex)
			{
				motor->internal.manual_run = 1;
			}
			if (motor->cmd.stop || motor->status.fault || motor->status.emergency_stop || motor->status.interlock || motor->status.mutex)
			{
				motor->internal.manual_run = 0;
			}
			motor->status.run = motor->internal.manual_run || motor->internal.jog_run;
		}
	}


	motor->status.state = motor->status.run | (motor->status.run_feedback << 1) | (motor->status.fault_feedback << 2) | (motor->status.mutex << 3) | 
		(motor->status.fault << 4) | (motor->status.interlock << 5) | (motor->status.emergency_stop << 6);
	
	motor->process.error = (motor->status.state & FAULT_BITS)?1:0;
	
	/*Reset command*/
	reset_cmd(&(motor->cmd));
	
	/*Output*/
	if (motor->cmd.simulation)
	{
		motor->io.run = 0;
	}
	else
	{
		motor->io.run = motor->status.run;
	}
	
	return 1;
}

/*Reset command*/
static void reset_cmd(struct InvertMotorCmd* cmd)
{
	if (cmd->start)
	{
		cmd->start = 0;
	}
	if (cmd->stop)
	{
		cmd->stop = 0;
	}
	if (cmd->jog)
	{
		cmd->jog = 0;
	}
	if (cmd->reset)
	{
		cmd->reset = 0;
	}
}

