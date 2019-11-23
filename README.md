# UDrive
Software Controls for the UDrive Motor Controller

## GUI Installation Instructions
**NOTE: SparkFun SAMD21 32-bit ARM Cortex-M0+ does NOT have driver support for Windows 8**

This installation will use Anaconda because it allows easy installation of PyQt4 with Python 2.7 (which are both older versions).

1. Download and install [Anaconda](https://www.anaconda.com/distribution/) for Python 2.7
2. Open "Anaconda Powershell Prompt (Anaconda2)"
3. Install PyQt4 by typing `pip install -i https://pypi.anaconda.org/ales-erjavec/simple pyqt4`
4. Install other packages that the GUI relies on by typing `pip install termcolor pyserial matplotlib pyqtgraph`

## GUI Run Instructions
1. Open "Anaconda Powershell Prompt (Anaconda2)"
2. In the Anaconda prompt, navigate to the 'GUI Files' directory
3. To start the GUI and connect to the SAMD21 board, type `python main.py`
4. Click on 'File' then 'Connect To UDrive' and select the correct port that the SAMD21 board is connected to.

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
