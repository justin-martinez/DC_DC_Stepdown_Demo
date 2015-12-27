# DC_DC_Stepdown_Demo

ATmega32U4 micro-controller to DC-DC Boost Converter 3V Up 5V to 9V 2A USB Output (module from China).

DC-DC Step Down Adjustable Power Supply Module
 
DC to DC Step-Down Specifications:

Module Properties: Non-isolated step-down module (BUCK)

Rectification: Non-synchronous rectifier

Input Voltage: DC 5V-30V

Output Voltage: DC 0.8V-24V (Adjustable, Output < Input)

Output Current: Rated current is 2.5A, (5A Max, If current is larger than 2.5A, enhancement of heat dissipation is required.)

Output Ripple: 30 mV(maximum)

Load Regulation: ±0.5%

Voltage Regulation: ±2.5%

Operating Temperature: -40°ƒ ~ +85°ƒ

Demo:

Use the ADC and serial of a micro- controller to display the output voltage on the terminal. We will watch the voltage swing while turning the potentiometer on the module. 

Tools: 
•	DC-DC Step Down Adjustable Power Supply Module

•	Male header 

•	Female to female jumper wires

•	Teensy 2.0 microcontroller

•	USB to USB-mini

•	DMM

•	Flat-head screwdriver

Steps:

1.	Solder male header pins to IN+, IN-, OUT+, OUT- on the module.

Figure 1

2.	Using the jumper wires connect:

	Teensy Pin		DC to DC Pin
	
•	Vcc 		to 	IN+

•	Gnd 		to 	{IN- , OUT-}

•	F1 		to 	OUT+

Example wiring:
 
3.	Load the example code DC_DC_Stepdown_Demo.hex on to the teensy using the Teensy bootloader.

4.	Open a USB serial communication terminal software (I recommend Termite). Watch the ADC voltage readings from the DC to DC step down module change as you turn the potentiometer. 

  
5.	Congratulations you have now successfully stepped down the 5V input voltage. 


