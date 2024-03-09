
PROGRAM _INIT
	(* Insert code here *)
	 
END_PROGRAM

PROGRAM _CYCLIC
	(* Insert code here *)
	IF clock_ms() - out_delay >= T#1s THEN
		out := NOT out;
		out_delay := clock_ms();
	END_IF;
	
	
	SensorHandler(TT001, TT001_PARA);
	SensorHandler(TT002, TT002_PARA);
	SensorHandler(TT003, TT003_PARA);
	
	analog_value := Unscale(value, 100.0, 0.0, SIS_HIGH_LIMIT, SIS_LOW_LIMIT);
END_PROGRAM

PROGRAM _EXIT
	(* Insert code here *)
	 
END_PROGRAM

