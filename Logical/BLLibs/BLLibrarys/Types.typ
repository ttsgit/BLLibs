
TYPE
	Sensor : 	STRUCT 
		io : SensorIO;
		cmd : SensorCmd;
		status : SensorStatus;
		internal : SensorInternal;
	END_STRUCT;
	SensorIO : 	STRUCT 
		analog_signal : INT;
	END_STRUCT;
	SensorStatus : 	STRUCT 
		value : REAL;
		gradient_value : REAL; (*ex 1cm / 10s = 0.1 cm/s*)
		state : WORD; (*Bit0: High alarm, Bit1: Low alarm, Bit2: High warnning, Bit3: Low warning, Bit4: High tolerance, Bit5: Low tolerance, Bit6: Gradient alarm, Bit7: Overflow, Bit8: Underflow*)
		high_alarm : BOOL;
		low_alarm : BOOL;
		high_warning : BOOL;
		low_warning : BOOL;
		high_tolerance : BOOL;
		low_tolerance : BOOL;
		gradient_alarm : BOOL;
		overflow : BOOL;
		underflow : BOOL;
	END_STRUCT;
	SensorInternal : 	STRUCT 
		high_alarm_timer : TIME;
		low_alarm_timer : TIME;
		high_warning_timer : TIME;
		low_warning_timer : TIME;
		high_tolerance_timer : TIME;
		low_tolerance_timer : TIME;
		gradient_timer : TIME;
		pre_value : REAL;
		delta_value : REAL;
	END_STRUCT;
	SensorCmd : 	STRUCT 
		simulation_value : REAL; (*Under simulation mode, the process value will use the simulation value*)
		out_service : BOOL; (*This mode is intended for purposes of maintenance and servicing (replacing the device, for example)*)
		simulation : BOOL; (*This mode is intended for purposes of testing processes or logics (interlock, for example).*)
	END_STRUCT;
	SensorPara : 	STRUCT 
		high_range : REAL; (*High scale range for the process value*)
		low_range : REAL; (*Low scale range for the process value*)
		high_alarm_limit : REAL; (*Alarm high*)
		low_alarm_limit : REAL; (*Alarm low*)
		high_warning_limit : REAL; (*Warning high*)
		low_warning_limit : REAL; (*Warning low*)
		high_tolerance_limit : REAL; (*Tolerance high*)
		low_tolerance_limit : REAL; (*Tolerance low*)
		gradient_limit : REAL; (*Positive or negative gradient limit*)
		gradient_cycle : DINT := 10; (*Gradient cycle time*)
		alarm_delay : DINT := 5; (*Alarm delay to outupt *)
		physical_unit : INT; (*Physical unit*)
		high_alarm_enable : BOOL; (*Enable alarm high*)
		low_alarm_enable : BOOL; (*Enable alarm low*)
		high_warning_enable : BOOL; (*Enable warning high*)
		low_warning_enable : BOOL; (*Enable warning low*)
		high_tolerance_enable : BOOL; (*Enable tolerance high*)
		low_tolerance_enable : BOOL; (*Enable tolerance low*)
		gradient_enable : BOOL; (*Enable gradient limit*)
		is_sis : BOOL; (*The analog range value of SIS module is 4000-20000, which is different from normal(0-32767).*)
		inverse : BOOL; (*Inverse the low and high range*)
	END_STRUCT;
	Valve : 	STRUCT 
		process : ValveProcess;
		io : ValveIO;
		cmd : ValveCmd;
		status : ValveStatus;
		internal : ValveInternal;
	END_STRUCT;
	ValveProcess : 	STRUCT 
		interlock_signal : BOOL;
		auto_open_signal : BOOL;
		stop_signal : BOOL;
		mutex_signal : BOOL;
		open_feedback_state : BOOL;
		close_feedback_state : BOOL;
		error : BOOL;
	END_STRUCT;
	ValveIO : 	STRUCT 
		open_feedback : BOOL;
		close_feedback : BOOL;
		valve_open : BOOL;
		emergency_stop : BOOL;
	END_STRUCT;
	ValveStatus : 	STRUCT 
		state : WORD; (*Bit0: Open, Bit1: Open feedback, Bit2: Close feedback, Bit3: mutex, Bit4: Open fault, Bit5: Close fault, Bit6: Interlock, Bit7: Emergency stop*)
		mode : BYTE; (*0: Manual mode 1: Automatic mode 2: Local mode 3: Out of service mode*)
		open : BOOL;
		mutex : BOOL;
		interlock : BOOL;
		emergency_stop : BOOL;
		open_fault : BOOL;
		close_fault : BOOL;
		open_feedback : BOOL;
		close_feedback : BOOL;
	END_STRUCT;
	ValveInternal : 	STRUCT 
		open_fault_timer : TIME;
		close_fault_timer : TIME;
		manual_open : BOOL;
		mode_edge : BOOL;
	END_STRUCT;
	ValveCmd : 	STRUCT 
		open_fault_delay : DINT := 5;
		close_fault_delay : DINT := 5;
		mode : BYTE; (*0: Manual mode 1: Automatic mode 2: Local mode 3: Out of service mode*)
		open : BOOL;
		close : BOOL;
		reset : BOOL;
		open_feedback_enable : BOOL := TRUE;
		close_feedback_enable : BOOL := TRUE;
		mutex_enable : BOOL := TRUE;
		interlock_enable : BOOL := TRUE;
		is_interlock_open : BOOL;
		simulation : BOOL;
	END_STRUCT;
	Regulator : 	STRUCT 
		process : RegulatorProcess;
		io : RegulatorIO;
		cmd : RegulatorCmd;
		status : RegulatorStatus;
		internal : RegulatorInternal;
	END_STRUCT;
	RegulatorProcess : 	STRUCT 
		auto_position : REAL;
		interlock_signal : BOOL;
		stop_signal : BOOL;
		mutex_signal : BOOL;
		error : BOOL;
	END_STRUCT;
	RegulatorIO : 	STRUCT 
		position_feedback : INT;
		position : INT;
		emergency_stop : BOOL;
	END_STRUCT;
	RegulatorStatus : 	STRUCT 
		position : REAL;
		position_feedback : REAL;
		state : WORD; (*Bit0: Open, Bit1: Open feedback, Bit2: Close feedback, Bit3: mutex, Bit4: Open fault, Bit5: Close fault, Bit6: Interlock, Bit7: Emergency stop*)
		mode : BYTE; (*0: Manual mode 1: Automatic mode 2: Local mode 3: Out of service mode*)
		mutex : BOOL;
		interlock : BOOL;
		emergency_stop : BOOL;
		feedback_fault : BOOL;
	END_STRUCT;
	RegulatorInternal : 	STRUCT 
		feedback_fault : BOOL;
		feedback_fault_timer : TIME;
	END_STRUCT;
	RegulatorCmd : 	STRUCT 
		position : REAL;
		fault_position_delta : REAL := 20.0;
		fault_delay : DINT := 5;
		mode : BYTE; (*0: Manual mode 1: Automatic mode 2: Local mode 3: Out of service mode*)
		reset : BOOL;
		feedback_enable : BOOL := TRUE;
		mutex_enable : BOOL := TRUE;
		interlock_enable : BOOL := TRUE;
		is_interlock_open : BOOL;
		simulation : BOOL;
		inverse : BOOL;
	END_STRUCT;
END_TYPE
