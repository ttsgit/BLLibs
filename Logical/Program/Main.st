
PROGRAM _INIT
	(* Insert code here *)
	 
END_PROGRAM

PROGRAM _CYCLIC
	(* Insert code here *)
	IF clock_ms() - out_delay >= T#1s THEN
		out := NOT out;
		out_delay := clock_ms();
	END_IF;
	
	array_output[0] := input.0;
	array_output[1] := input.1;
	array_output[2] := input.2;
	array_output[3] := input.3;
	array_output[4] := input.4;
	array_output[5] := input.5;
	array_output[6] := input.6;
	array_output[7] := input.7;
	
	output.0 := array_input[0];
	output.1 := array_input[1];
	output.2 := array_input[2];
	output.3 := array_input[3];
	output.4 := array_input[4];
	output.5 := array_input[5];
	output.6 := array_input[6];
	output.7 := array_input[7];
	
	connected := module_ok;
	
	SensorHandler(TT001, TT001_PARA);
	SensorHandler(TT002, TT002_PARA);
	SensorHandler(TT003, TT003_PARA);
	
	analog_value := Unscale(value, 100.0, 0.0, SIS_HIGH_LIMIT, SIS_LOW_LIMIT);
	
	ValveHandler(valve001);
	ValveHandler(valve002);
	
	RegulatorHandler(regulator001);
	RegulatorHandler(regulator002);
	
	MotorHandler(motor001);
	MotorHandler(motor002);
	
	InvertMotorHandler(motor101);
	InvertMotorHandler(motor102);
END_PROGRAM

PROGRAM _EXIT
	(* Insert code here *)
	 
END_PROGRAM

