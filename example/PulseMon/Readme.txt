****************************************************
            Pulse Counter / Monitor
****************************************************
Device Used: PIC16F84
Author:      Michael Pearce
Started:     14 November 2001
****************************************************

**** Description ****

This is a simple program that monitors the frequency 
on the T0CLK pin and if the frequency is less than 
a pre-set value then the current Date/Time (From the 
PCF8583 Calander/Clock I2C device) and the recorded
frequency is output to the serial port.

This program was done to monitor the water flow feeding
a cooler system to find any flow drop outs that were 
possibly causing a system shutdown. 


**** How it works - in brief ****

TMR0 is configured to count using the external pin as
the counters clock. 

The PCF8584 is configured to output an interrupt pulse
once per second, and is connected to the INT pin which
is set up as input and to generate an interrupt.

When the 1 second interrupt occurs, the current TMR0
value is stored, then TMR0 is cleared to start the next 
count. A Flag is tehn set to indicate to the main loop
that a new TMR value has been read and is ready to be
processed.

Because the value of TMR0 is read once per second, its
value is actual Pulses/Sec or Hz. 

When the flag is detectoed in the main loop then the 
new value is compared with the stored Limit value.
If the new value is less than the limit then the date
and time is read from the PCF8583 Calendar/ Clock chip
using I2C and the Date/Time and recorded level are output
to the serial port using a software driven routine.

It also has a simple User interface to allow the limit, 
date and time to be set up and stored by using a serial
terminal.


**** Used in this program **** 

Timer 0 using external clock input
Interrupt input pin 
EEPROM read/write 
I2C Read/Write    
RS-232 Tx/Rx
Watchdog timer.
Interrupt routine 

BCD Value -> ASCII conversion
ASCII -> BCD Value Conversion


**** Some other uses with a little modification **** 

Flow System alarm
 By using another I/O port as an output to drive a
 siren and/or light an alarm could be sounded when
 the input frequency drops below a certian level.

Emergency  Shutdown
 Use another I/O port to trip a shutdown relay when
 the frequency drops below the set level.

Note: Can change so it trips when it goes ABOVE the 
level for other uses.

