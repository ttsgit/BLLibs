
#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "BLLibrarys.h"
#ifdef __cplusplus
	};
#endif


/* TODO: This is a function that handles sensor */
plcbit SensorHandler(struct Sensor* sensor, struct SensorPara* sensor_para)
{	
	/*Out of service*/
	if (sensor->cmd.out_service)
	{
		sensor->status.overflow = 0;
		sensor->status.underflow = 0;
		sensor->status.value = 0;
		sensor->status.gradient_value = 0;
		sensor->status.high_alarm = 0;
		sensor->status.low_alarm = 0;
		sensor->status.high_warning = 0;
		sensor->status.low_warning = 0;
		sensor->status.high_tolerance = 0;
		sensor->status.low_tolerance = 0;
		sensor->status.gradient_alarm =  0;
		sensor->status.state = 0;
		return 0;
	}

	/*Simulation*/
	if (sensor->cmd.simulation)
	{
		sensor->status.value = sensor->cmd.simulation_value;
	}
	else
	{
		/*Get process value*/
		if(sensor_para->is_sis)
		{
			if(sensor_para->inverse)
				sensor->status.value = Scale(sensor->io.analog_signal, sensor_para->low_range, sensor_para->high_range, SIS_HIGH_LIMIT, SIS_LOW_LIMIT);
			else
				sensor->status.value = Scale(sensor->io.analog_signal, sensor_para->high_range, sensor_para->low_range, SIS_HIGH_LIMIT, SIS_LOW_LIMIT);
		}
		else
		{
			if (sensor_para->inverse)
				sensor->status.value = Scale(sensor->io.analog_signal, sensor_para->low_range, sensor_para->high_range, HIGH_LIMIT, LOW_LIMIT);
			else	
				sensor->status.value = Scale(sensor->io.analog_signal, sensor_para->high_range, sensor_para->low_range, HIGH_LIMIT, LOW_LIMIT);
		}		
	}

	/*Overflow*/
	sensor->status.overflow = (sensor->io.analog_signal > HIGH_LIMIT)?1:0;
	
	/*Underflow*/
	sensor->status.underflow = (sensor->io.analog_signal < (LOW_LIMIT - DEADBAND_LIMIT))?1:0;
	
	/*High alarm*/
	plcbit high_alarm = sensor_para->high_alarm_enable?(sensor->status.value > sensor_para->high_alarm_limit):0;
	sensor->status.high_alarm = OutputDelay(high_alarm, &sensor->process.high_alarm_timer, sensor_para->alarm_delay);
	
	/*Low alarm*/
	plcbit low_alarm = sensor_para->low_alarm_enable?(sensor->status.value < sensor_para->low_alarm_limit):0;
	sensor->status.low_alarm = OutputDelay(low_alarm, &sensor->process.low_alarm_timer, sensor_para->alarm_delay);
	
	/*High warning*/
	plcbit high_warning = sensor_para->high_warning_enable?(sensor->status.value > sensor_para->high_warning_limit):0;
	sensor->status.high_warning = OutputDelay(high_warning, &sensor->process.high_warning_timer, sensor_para->alarm_delay);

	/*Low warning*/
	plcbit low_warning = sensor_para->low_warning_enable?(sensor->status.value < sensor_para->low_warning_limit):0;
	sensor->status.low_warning = OutputDelay(low_warning, &sensor->process.low_warning_timer, sensor_para->alarm_delay);
	
	/*High tolerance*/
	plcbit high_tolerance = sensor_para->high_tolerance_enable?(sensor->status.value > sensor_para->high_tolerance_limit):0;
	sensor->status.high_tolerance = OutputDelay(high_tolerance, &sensor->process.high_tolerance_timer, sensor_para->alarm_delay);
	
	/*Low tolerance*/
	plcbit low_tolerance = sensor_para->low_tolerance_enable?(sensor->status.value < sensor_para->low_tolerance_limit):0;
	sensor->status.low_tolerance = OutputDelay(low_tolerance, &sensor->process.low_tolerance_timer, sensor_para->alarm_delay);

	/*Gradient alarm*/
	if (sensor_para->gradient_enable)
	{
		if (sensor_para->gradient_cycle <= 0)
		{
			sensor_para->gradient_cycle = 1;
		}
		if ((clock_ms() - sensor->process.gradient_timer) >= (sensor_para->gradient_cycle * 1000))
		{
			float temp = sensor->status.value - sensor->process.pre_value;
			sensor->status.gradient_value = temp / (float)sensor_para->gradient_cycle;
			temp = (temp < 0.0f)?(temp * -1.0f):temp;
			sensor->status.gradient_alarm = (temp > sensor_para->gradient_limit)?1:0;
			sensor->process.pre_value = sensor->status.value;
			sensor->process.gradient_timer = clock_ms();
		}
	}
	else
	{
		sensor->status.gradient_alarm = 0;
	}
	
	sensor->status.state = (sensor->status.high_alarm | (sensor->status.low_alarm << 1) | (sensor->status.high_warning << 2)  | (sensor->status.low_warning << 3) | 
		(sensor->status.high_tolerance << 4) | (sensor->status.low_tolerance << 5)  | (sensor->status.gradient_alarm << 6)  | (sensor->status.overflow << 7) | (sensor->status.underflow << 8));
	
	return 1;
}

