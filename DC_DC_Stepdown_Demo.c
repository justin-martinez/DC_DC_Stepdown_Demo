/*
 * DC_DC_Stepdown_Demo.c
 *
 * Created: 8/14/2015 2:09:01 PM
 *  Author: martijus
	This code has two main functional goals: First it will read in ADC voltage values from the DC to DC step down module. 
	Second it will output the readings from the USB mini port on the Teensy through serial. We use the library usb_serial, 
	which was open source from PJRC. 
 */ 

// #defines:__ ********************
#define F_CPU           16000000UL					// F_CPU is used by util/delay.h to define what Hz the CPU is running at. 
#define LED_ON		(PORTD |= (1<<6))				// Hot Key for easy reading of functionality. 
#define LED_OFF		(PORTD &= ~(1<<6))				// Hot Key for easy reading of functionality.
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))	// Setting up Hz for serial communication. 


// #includes:__ ********************
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <util/delay.h>
#include "usb_serial.h"


// Function declorations:__ ********************
void adc_init();
void led_init(void);
uint16_t adc_read(void);
void send_str(const char *s);
void send_strB(char *s);
void send_serial (uint16_t out);


// ******************** Main ********************
int main(void)
{
	// variables:
	uint16_t data;
	uint16_t voltage;
	uint16_t Vref = 5;
	
	//setup:
	CPU_PRESCALE(0);	// set serial communication for 16 MHz clock.
	adc_init();
	usb_init();	
	
    while(1)	// creating an constant cycle of reading the ADC and outputting the reading through serial
    {
		data = adc_read();	// Taking an ADC reading.
		
		/* Vref = 5v, ADC divides 0-5v into 1024 (2^10) sections. Thus each ADC value is 5/1024= 0.004883V  
		We multiply our ADC reading by 1000 to get our output into the mV range. */
		voltage= (data * .004883)*1000;		
		
		send_serial(voltage);	// outputs reading through serial.
    }
}

// ******************** Functions ********************
void adc_init() {
	/* Digital Input Disable Register 0 – DIDR0
	Bits {7:4, 1:0}	- When these bits are written logic one, the digital input buffer on the corresponding ADC pin is disabled.
	The corresponding PIN Register bit will always read as zero when this bit is set.
	0 - ADC0D -> F0
	1 - ADC1D -> F1
	2 - NULL
	3 - NULL
	4 - ADC4D -> F4
	5 - ADC5D -> F5
	6 - ADC6D -> F6
	7 - ADC7D -> F7	 
	*/
	DIDR0 = 0x01;  // 00000001 is the binary equivalent of 0x01. Setting the register to 00010011 disables digital input, making the pin an analog pin. ADC pin 0 is F0.
	
	/*ADC Multiplexer Selection Register – ADMUX
	Bits {7:0}
	7 - REFS1: select the voltage reference for the ADC
	6 - REFS0: select the voltage reference for the ADC
	5 - ADLAR: ADC Left Adjust Result. The ADLAR bit affects the presentation of the ADC conversion result in the ADC Data Register.
		Write one to ADLAR to left adjust the result. Otherwise, the result is right adjusted. Changing the
		ADLAR bit will affect the ADC Data Register immediately, regardless of any ongoing conversions.
	4 - Analog Channel Selection Bit
	3 - Analog Channel Selection Bit
	2 - Analog Channel Selection Bit
	1 - Analog Channel Selection Bit
	0 - Analog Channel Selection Bit
	Bits 4:0 – MUX4:0: Analog Channel Selection Bits (See p.308 of data sheet for table)  
	The value of these bits selects which combination of analog inputs are connected to the ADC.
	These bits also select the gain for the differential channels. If these bits are changed during a conversion, 
	the change will not go in effect until this conversion is complete (ADIF in ADCSRA is set).
	*/	
	ADMUX |= (1 << REFS0); // 01000000 sets the register to keep right adjust on, and selects ADC0 and only input channel. 
	
	
	/*ADC Control and Status Register A – ADCSRA
	Bits {7:0}
		7 - ADEN: ADC Enable. 
			Writing this bit to one enables the ADC. By writing it to zero, the ADC is turned off. Turning the
			ADC off while a conversion is in progress, will terminate this conversion.
		6 - ADSC: ADC Start Conversion
			In Single Conversion mode, write this bit to one to start each conversion. In Free Running mode,
			write this bit to one to start the first conversion. The first conversion after ADSC has been written
			after the ADC has been enabled, or if ADSC is written at the same time as the ADC is enabled,
			will take 25 ADC clock cycles instead of the normal 13. This first conversion performs initialization
			of the ADC.
			ADSC will read as one as long as a conversion is in progress. When the conversion is complete,
			it returns to zero. Writing zero to this bit has no effect.
		5 - ADATE: ADC Auto Trigger Enable
			When this bit is written to one, Auto Triggering of the ADC is enabled. The ADC will start a conversion
			on a positive edge of the selected trigger signal. The trigger source is selected by setting
			the ADC Trigger Select bits, ADTS in ADCSRB.
		4 - ADIF: ADC Interrupt Flag
			This bit is set when an ADC conversion completes and the Data Registers are updated. The
			ADC Conversion Complete Interrupt is executed if the ADIE bit and the I-bit in SREG are set.
			ADIF is cleared by hardware when executing the corresponding interrupt handling vector. Alternatively,
			ADIF is cleared by writing a logical one to the flag. Beware that if doing a Read-Modify-Write on ADCSRA, 
			a pending interrupt can be disabled. This also applies if the SBI and CBI instructions are used.
		3 - ADIE: ADC Interrupt Enable
			When this bit is written to one and the I-bit in SREG is set, the ADC Conversion Complete Interrupt
			is activated.
		2 - ADC Prescaler Select Bits
		1 - ADC Prescaler Select Bits
		0 - ADC Prescaler Select Bits
		ADPS2:0: ADC Prescaler Select Bits
		These bits determine the division factor between the XTAL frequency and the input clock to the
		ADC. (See data sheet p.311 for table)
	*/
	// Setting the register to 11000111. Enables ADC and configures ADC prescaler to a 128 division factor. 
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}



void led_init(void){
	DDRD |= (1<<6);	// setting the Teensy LED active. 
	LED_ON;
	while (!usb_configured()){	// The LED goes high while trying to connect serial. It will go low again when connected. 
		_delay_ms(1000);
	}
}




uint16_t adc_read(void){
	uint16_t data;
	uint8_t low;
	_delay_ms(1000); // only reads every second. 
	/* ADSC bit in the ADCSRA register is toggled high to initiate an ADC read.
	   Since we have already selected F0 as our only ADC read pin the register
	   will read from that pin only. */ 
	ADCSRA |= (1<<ADSC); 
	
	/* Wait for a Valid Sample. 0x10 = 00010000 = ADATE  
	The ADATE bit in register ADCSRA is an auto trigger enable for the register. When conversion is complete it will go low.*/
	while ((ADCSRA & 0x10) == 0 );  
	
	/*After getting a valid read you have to toggle the interrupt flag ADIF in the ADCSRA register low again.
	  If we were using interrupts the flag would be reset automatically.  */
	ADCSRA |= (1<<ADIF); 
	
	/*We create the dummy variable low because if we set data= ((ADCH<<8) | ADCL); the register will read the lower bits first.
	for our reading we need it to see the ADCH bits first. By setting the low bits into an int we can get the compiler to set 
	all the bits in the correct order. */
	
	low = ADCL; // Must use to adjust bits in following equation. 
	//ADC has two 8 bit registers to take in a reading, ADCH for the high bits and ADCL for the low bits.
	data = ((ADCH<<8) | low);
	 
	return (data);
}

// this function takes all the characters and feeds them into the usb_serial data stream.
void send_strB(char *s){
	int i = 0;
	while (s[i] != '\0') {	
		usb_serial_putchar(s[i]);
		i++;
	}
}



void send_serial (uint16_t out){
	char temp_string[32];
	usb_serial_flush_input();	// we empty the data line before sending.
	
	// sprintf will take multiple data types and turn the whole line into a string.
	sprintf(temp_string, "the voltage is %u mV \n", out);	
	send_strB(temp_string);
}
