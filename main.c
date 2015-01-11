#include <avr/io.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <avr/interrupt.h> 

// just in case that we need a timer
#define F_CPU 16000000
#include <util/delay.h>

#define fr 12500  // 12,5 kHz
#define M 64      // 1:64 prescaler
#define ZF 21600000  // 21,6 MHz ZF

#define MAX_CHANNELS 256 // maximum possible channels, 160 are only 
                         // possible without bigger hardware mod....

// defining the prescaler 0x03C to be copied to the PLL
// depends on 750000/fr
#define L5 0xC
#define L6 0x3
#define L7 0x0


// the following two structs are to construct the frequency 
// freq_int is holding ur configuration while freq_hex is calculated 
// during init 
struct freq_int_t {
	uint16_t rx_freq;
	uint16_t tx_freq; 
	uint16_t ctcss; 
}

freq_int_t freq_int[MAX_CHANNELS+1]={
	{438950000, 431350000, 1622},
	{433500000, 433500000, 0},
	{NULL, NULL, NULL}
};


struct freq_hex_t {
	unsigned char rx_freq[8];
	unsigned char tx_freq[8]; 
}
 
freq_hex_t freq_hex[MAX_CHANNELS+1];



ISR(PCINT1_vect) {
	// insert pin change here 
}


int main (void) {            

	PCMSK1 |= (1<<PCINT1);
	PCMSK4 |= (1<<PCINT4);
	sei(); 


	

   while(1) {                // (5)
     /* "leere" Schleife*/   // (6)
   }     
 
   /* wird nie erreicht */
   return 0;                 // (6)
}
