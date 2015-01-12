#include <avr/io.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <avr/interrupt.h> 
#include <avr/sleep.h>
#include <avr/pgmspace.h>

// just in case that we need a timer
#ifndef F_CPU
#define F_CPU 16000000
#endif
#include <util/delay.h>

#define fr 12500  // 12,5 kHz
#define M 64      // 1:64 prescaler
#define ZF 21600000  // 21,6 MHz ZF

#define MAX_CHANNELS 256 // maximum possible channels, 160 are only 
                         // possible without bigger hardware mod....

// defining the prescaler 0x03C to be copied to the PLL
// depends on 750000/fr
#define L5 (uint8_t) 0x0C
#define L6 (uint8_t) 0x03
#define L7 (uint8_t) 0x00


// the following two structs are to construct the frequency 
// freq_int is holding ur configuration while freq_hex is calculated 
// during init 
struct freq_int_t {
	uint32_t rx_freq;
	uint32_t tx_freq; 
	uint16_t ctcss; 
};

struct freq_int_t const freq_int[MAX_CHANNELS+1] PROGMEM={
	{438950000, 431350000, 1622},
	{433500000, 433500000, 0},
	{0, 0, 0}
};


struct freq_hex_t {
	uint8_t rx_freq[8];
	uint8_t tx_freq[8]; 
};
 
struct freq_hex_t freq_hex;
uint16_t channel=MAX_CHANNELS; // this is to kick the setup_channel() initially
uint16_t num_channels=0; // this is to kick the setup_channel() initially


uint16_t calc_N (uint32_t freq) {
	return (uint16_t) (freq/((uint32_t) fr * (uint32_t) M));
}

uint16_t calc_A (uint32_t freq, uint16_t N) {
	return (freq/fr) - M * N;
}

uint8_t get_chr0 (uint16_t n) { return (uint8_t) (n & 0x000F); }
uint8_t get_chr1 (uint16_t n) { return (uint8_t) ((n & 0x00F0)>>4); }
uint8_t get_chr2 (uint16_t n) { return (uint8_t) ((n & 0x0F00)>>8); }

void setup_channel(uint16_t c) {
	uint16_t N;  
	uint16_t A;  
	if ( c == channel ) { return; } 
	if ( c >= num_channels ) { return; } // do nothing if c is 
								// higher than the last configured channel data
	// rx freq
	N=calc_N(freq_int[c].rx_freq-ZF);
	A=calc_A(freq_int[c].rx_freq-ZF, N);
	freq_hex.rx_freq[0]=get_chr0(A);
	freq_hex.rx_freq[1]=get_chr1(A);
	freq_hex.rx_freq[2]=get_chr0(N); 
	freq_hex.rx_freq[3]=get_chr1(N); 
	freq_hex.rx_freq[4]=get_chr2(N);
	freq_hex.rx_freq[5]=L5; 
	freq_hex.rx_freq[6]=L6; 
	freq_hex.rx_freq[7]=L7;
	
	// tx freq
	N=calc_N(freq_int[c].rx_freq);
	A=calc_A(freq_int[c].rx_freq, N);
	freq_hex.tx_freq[0]=get_chr0(A);
	freq_hex.tx_freq[1]=get_chr1(A);
	freq_hex.tx_freq[2]=get_chr0(N); 
	freq_hex.tx_freq[3]=get_chr1(N); 
	freq_hex.tx_freq[4]=get_chr2(N);
	freq_hex.tx_freq[5]=L5; 
	freq_hex.tx_freq[6]=L6; 
	freq_hex.tx_freq[7]=L7; 
	
	channel=c; 
}

// here the pin change is calculated // 
ISR(PCINT0_vect) {
	uint8_t i=PINB&(0x07);
	uint8_t ptt=(PINB&(0x08))>>3;
	uint16_t ch=(PINB&(0xF0))>>4;

	setup_channel(ch); 
	
	if ( ptt != 0x01 ) {
		PORTD=freq_hex.rx_freq[i];
	} else {
		PORTD=freq_hex.tx_freq[i];
	}
}


int main (void) {            
	uint8_t i;
	uint8_t ptt;
	uint16_t ch;

	//DDRA=0b00000000;
	DDRB=0b00000000; // all pins inbound 5th bit is ptt. 
	DDRC=0b11111111; // outbound 
	DDRD=0b11111111; // PD0-3 are outbound 
	//PORTA=0b11111111; // all pins pull up
	PORTB=0b11111111; // all pins pull up
	//PORTD=0b11111111; // all pins pull up

	// initialize num channels first... 
	while (freq_int[num_channels].rx_freq != 0) { num_channels++; }
	setup_channel(0);
	
	PCICR = 0x01;
	PCMSK0 = 0x01;
	sei();

   while(1) {                // (5)
	/*	i=PINB&(0x07);
		ptt=(PINB&(0x08))>>3;
		ch=(PINB&(0xF0))>>4;
	
		setup_channel(ch); 
	
		if ( ptt != 0x01 ) {
			PORTD=freq_hex.rx_freq[i];
		} else {
			PORTD=freq_hex.tx_freq[i];
		}	*/
		//sleep_bod_disable();
	//	sleep_enable();
	//	sleep_cpu();
//		sleep_disable();
   }     
 
   /* wird nie erreicht */
   return 0;                 // (6)
}
