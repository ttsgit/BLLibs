
#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "BLLibrarys.h"
#ifdef __cplusplus
	};
#endif


plcbit alarm_delay(plcbit alarm, plctime* timer, plctime delay);


/* TODO: This is a function that handles sensor */
plcbit SensorHandler(struct Sensor* sensor, struct SensorPara* sensor_para)
{
	plcbit high_alarm, low_alarm, high_warning, low_warning, high_tolerance, low_tolerance;
	
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
	high_alarm = sensor_para->high_alarm_enable?(sensor->status.value > sensor_para->high_alarm_limit):0;
	sensor->status.high_alarm = alarm_delay(high_alarm, &sensor->process.high_alarm_timer, sensor_para->alarm_delay);
	
	/*Low alarm*/
	low_alarm = sensor_para->low_alarm_enable?(sensor->status.value < sensor_para->low_alarm_limit):0;
	sensor->status.low_alarm = alarm_delay(low_alarm, &sensor->process.low_alarm_timer, sensor_para->alarm_delay);
	
	/*High warning*/
	high_warning = sensor_para->high_warning_enable?(sensor->status.value > sensor_para->high_warning_limit):0;
	sensor->status.high_warning = alarm_delay(high_warning, &sensor->process.high_warning_timer, sensor_para->alarm_delay);

	/*Low warning*/
	low_warning = sensor_para->low_warning_enable?(sensor->status.value < sensor_para->low_warning_limit):0;
	sensor->status.low_warning = alarm_delay(low_warning, &sensor->process.low_warning_timer, sensor_para->alarm_delay);
	
	/*High tolerance*/
	high_tolerance = sensor_para->high_tolerance_enable?(sensor->status.value > sensor_para->high_tolerance_limit):0;
	sensor->status.high_tolerance = alarm_delay(high_tolerance, &sensor->process.high_tolerance_timer, sensor_para->alarm_delay);
	
	/*Low tolerance*/
	low_tolerance = sensor_para->low_tolerance_enable?(sensor->status.value < sensor_para->low_tolerance_limit):0;
	sensor->status.low_tolerance = alarm_delay(low_tolerance, &sensor->process.low_tolerance_timer, sensor_para->alarm_delay);

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

/* Delay time to output alarm*/
plcbit alarm_delay(plcbit alarm, plctime* timer, plctime delay)
{
	plcbit alarm_delay;
	if(alarm)
	{
		if((clock_ms() - *timer) >= (delay * 1000))
		{
			alarm_delay = 1;
		}	
	}
	else
	{
		alarm_delay = 0;
		*timer = clock_ms();
	}
	return alarm_delay;
}
