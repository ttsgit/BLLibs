
{REDUND_OK} FUNCTION SensorHandler : BOOL (*TODO: This is a function that handles sensor*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		sensor : Sensor;
		sensor_para : SensorPara;
	END_VAR
END_FUNCTION

{REDUND_OK} FUNCTION Scale : REAL (*TODO:min-max normalization*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		anolog : INT;
		high_range : REAL;
		low_range : REAL;
		analog_high : INT;
		analog_low : INT;
	END_VAR
END_FUNCTION

{REDUND_OK} FUNCTION Unscale : INT (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		value : REAL;
		high_range : REAL;
		low_range : REAL;
		analog_high : INT;
		analog_low : INT;
	END_VAR
END_FUNCTION

{REDUND_OK} FUNCTION ValveHandler : BOOL (*TODO: Add your comment here*) (*$GROUP=User,$CAT=User,$GROUPICON=User.png,$CATICON=User.png*)
	VAR_INPUT
		valve : Valve;
	END_VAR
END_FUNCTION
