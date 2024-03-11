
#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
#include "BLLibrarys.h"
#ifdef __cplusplus
	};
#endif


#define FAULT_BITS 0xFFFC


static void reset_cmd(struct RegulatorCmd* cmd);


/* TODO: Add your comment here */
plcbit RegulatorHandler(struct Regulator* regulator)
{
	/*TODO: Add your code here*/
	/* Mode select*/
	regulator->status.mode = regulator->cmd.mode;
	
	/*Out of service mode*/
	if (regulator->status.mode == OUT_SERVICE_MODE)
	{
		regulator->status.position = 0.0f;
		regulator->status.position_feedback = 0.0f;
		regulator->status.feedback_fault = 0;
		regulator->status.emergency_stop = 0;
		regulator->status.interlock = 0;
		regulator->status.mutex = 0;
		regulator->status.state = 0;
		reset_cmd(&(regulator->cmd));
		return 0;
	}
	
	/*Local mode*/
	if(regulator->status.mode == LOCAL_MODE)
	{
		if (regulator->cmd.inverse)
		{
			regulator->status.position_feedback = Scale(regulator->io.position_feedback, REGULATOR_POSITION_LOW, REGULATOR_POSITION_HIGH, HIGH_LIMIT, LOW_LIMIT);
		}
		else
		{
			regulator->status.position_feedback = Scale(regulator->io.position_feedback, REGULATOR_POSITION_HIGH, REGULATOR_POSITION_LOW, HIGH_LIMIT, LOW_LIMIT);
		}
		regulator->status.position = regulator->status.position_feedback;
		regulator->status.emergency_stop = regulator->io.emergency_stop;
		regulator->status.interlock = 0;
		regulator->status.mutex = 0;
		regulator->status.feedback_fault = 0;
		regulator->status.state = regulator->status.mutex | (regulator->status.feedback_fault << 1) | (regulator->status.interlock << 2) | (regulator->status.emergency_stop << 3);
		reset_cmd(&(regulator->cmd));
		return 1;
	}
	
	/*Simulation*/
	if (regulator->cmd.simulation)
	{
		regulator->status.position_feedback = regulator->status.position;
	}
	else
	{
		if (regulator->cmd.feedback_enable)
		{
			/*Feedback position*/
			if (regulator->cmd.inverse)
			{
				regulator->status.position_feedback = Scale(regulator->io.position_feedback, REGULATOR_POSITION_LOW, REGULATOR_POSITION_HIGH, HIGH_LIMIT, LOW_LIMIT);
			}
			else
			{
				regulator->status.position_feedback = Scale(regulator->io.position_feedback, REGULATOR_POSITION_HIGH, REGULATOR_POSITION_LOW, HIGH_LIMIT, LOW_LIMIT);
			}
		}
		else
		{
			/*Feedback position*/
			regulator->status.position_feedback = 0.0f;
		}
	}
		
	/*Emergency stop*/
	if (regulator->io.emergency_stop | regulator->process.stop_signal)
	{
		regulator->status.emergency_stop = 1;
	}
	else
	{
		if(regulator->cmd.reset)
		{
			regulator->status.emergency_stop = 0;
		}
	}
	
	/*Interlock*/
	if (regulator->cmd.interlock_enable)
	{
		if (regulator->process.interlock_signal)
		{
			regulator->status.interlock = 1;
		}
		else
		{
			if(regulator->cmd.reset)
			{
				regulator->status.interlock = 0;
			}
		}
	}
	else
	{
		regulator->status.interlock = 0;
	}

	/*Mutex*/
	regulator->status.mutex = regulator->process.mutex_signal && regulator->cmd.mutex_enable;

	/*Feedback fault*/
	if (regulator->cmd.feedback_enable)
	{
		if (fabs(regulator->status.position - regulator->status.position_feedback) > regulator->cmd.fault_position_delta)
		{
			regulator->internal.feedback_fault = 1;
		}
		else
		{
			regulator->internal.feedback_fault = 0;
		}
	}
	else
	{
		regulator->internal.feedback_fault = 0;
	}
	regulator->status.feedback_fault = OutputDelay(regulator->internal.feedback_fault, &regulator->internal.feedback_fault_timer, regulator->cmd.fault_delay);
	
	/*Automatic mode*/
	if(regulator->status.mode == AUTOMATIC_MODE)
	{
		if (regulator->cmd.is_interlock_open)
		{
			if (regulator->status.interlock)
			{
				regulator->status.position = 100.0f;
			}
			else
			{
				if ((!regulator->status.emergency_stop) && (!regulator->status.mutex))
				{
					regulator->status.position = regulator->process.auto_position;
				}
				else
				{
					regulator->status.position = 0.0f;
				}	
			}
		}
		else
		{
			if ((!regulator->status.emergency_stop) && (!regulator->status.interlock) && (!regulator->status.mutex))
			{
				regulator->status.position = regulator->process.auto_position;
			}
			else
			{
				regulator->status.position = 0.0f;
			}
		}
	}
	
	/*Manual mode*/
	if (regulator->status.mode == MANUAL_MODE)
	{
		if (regulator->cmd.is_interlock_open)
		{
			if (regulator->status.interlock)
			{
				regulator->status.position = 100.0f;
			}
			else
			{
				if ((!regulator->status.emergency_stop) && (!regulator->status.mutex))
				{
					regulator->status.position = regulator->cmd.position;
				}
				else
				{
					regulator->status.position = 0.0f;
				}	
			}
		}
		else
		{
			if ((!regulator->status.emergency_stop) && (!regulator->status.interlock) && (!regulator->status.mutex))
			{
				regulator->status.position = regulator->cmd.position;
			}
			else
			{
				regulator->status.position = 0.0f;
			}
		}
	}


	regulator->status.state = regulator->status.mutex | (regulator->status.feedback_fault << 1) | (regulator->status.interlock << 2) | (regulator->status.emergency_stop << 3);
	regulator->process.error = (regulator->status.state & FAULT_BITS)?1:0;

	/*Reset command*/
	reset_cmd(&(regulator->cmd));
	
	/*Output*/
	if (regulator->cmd.simulation)
	{
		regulator->io.position = 0;
	}
	else
	{
		if (regulator->cmd.inverse)
		{
			regulator->io.position = Unscale(regulator->status.position, REGULATOR_POSITION_HIGH, REGULATOR_POSITION_LOW, LOW_LIMIT, HIGH_LIMIT);			
		}
		else
		{
			regulator->io.position = Unscale(regulator->status.position, REGULATOR_POSITION_HIGH, REGULATOR_POSITION_LOW, HIGH_LIMIT, LOW_LIMIT);
		}
	}
	
	return 1;
}

	/*Reset command*/
static void reset_cmd(struct RegulatorCmd* cmd)
{
	if (cmd->reset)
	{
		cmd->reset = 0;
	}
}
