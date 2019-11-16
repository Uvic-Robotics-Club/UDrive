# UDrive
Software Controls for the UDrive Motor Controller

## API
The format for sending information to the UDrive motor controller will always be in the format

(Mode)(Data)(CRC16)
Sending

| Mode | Description | Data Type | Data Description |
| --   | --          | --                |    --              |
| "M" | Manual Speed for both motors | float x 2 | speed of the motor where 0 is full reverse and 255 is full forward |
| "V" | Velocity Mode using encoders, Same speed for both motors (enables PID) | float x 2 | rpm for both motors |
| "E" | Enable/Disable PID | Bool | 0 = Disable, 1 = Enable |
| "P" | Set PID Values | float x 3 | Kp , Ki, Kd |
| "D" | Set Wheel Diameter | float | wheel diameter in m |
| "T" | Set encoder ticks per revolution | integer | number of ticks per output revolution |
| "A" | Set max acceleration | float | how fast the output can change measured in V/S |
