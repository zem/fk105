#include <avr/io.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <avr/interrupt.h> 
#include <avr/sleep.h>

// just in case that we need a timer
//#define F_CPU 16000000
//#include <util/delay.h>

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
	uint32_t rx_freq;
	uint32_t tx_freq; 
	uint16_t ctcss; 
};

struct freq_int_t freq_int[MAX_CHANNELS+1]={
	{438950000, 431350000, 1622},
	{433500000, 433500000, 0},
	{0, 0, 0}
};


struct freq_hex_t {
	unsigned char rx_freq[8];
	unsigned char tx_freq[8]; 
};
 
struct freq_hex_t freq_hex[MAX_CHANNELS+1];
int anz_channels=0; 


uint16_t calc_N (uint32_t freq) {
	return (uint16_t) (freq/((uint32_t) fr * (uint32_t) M));
}

uint16_t calc_A (uint32_t freq, uint16_t N) {
	return (freq/fr) - M * N;
}

unsigned char get_chr0 (uint16_t n) { return (unsigned char) (n & 0x000F); }
unsigned char get_chr1 (uint16_t n) { return (unsigned char) ((n & 0x00F0)>>1); }
unsigned char get_chr2 (uint16_t n) { return (unsigned char) ((n & 0x0F00)>>2); }

void setup_channels() {
	int i=0;
	uint16_t N;  
	uint16_t A;  
	for (i=0; i<MAX_CHANNELS; i++) {
		if ( freq_int[i].rx_freq == 0 ) { 
			anz_channels=i; 
			freq_hex[i]=(struct freq_hex_t) {0, 0}; 
			return; 
		}
		// rx freq
		N=calc_N(freq_int[i].rx_freq-ZF);
		A=calc_A(freq_int[i].rx_freq-ZF, N);
		freq_hex[i].rx_freq[0]=get_chr0(A);
		freq_hex[i].rx_freq[1]=get_chr1(A);
		freq_hex[i].rx_freq[2]=get_chr0(N); 
		freq_hex[i].rx_freq[3]=get_chr1(N); 
		freq_hex[i].rx_freq[4]=get_chr2(N);
		freq_hex[i].rx_freq[5]=L5; 
		freq_hex[i].rx_freq[6]=L6; 
		freq_hex[i].rx_freq[7]=L7;
		
		// tx freq
		N=calc_N(freq_int[i].rx_freq);
		A=calc_A(freq_int[i].rx_freq, N);
		freq_hex[i].tx_freq[0]=get_chr0(A);
		freq_hex[i].tx_freq[1]=get_chr1(A);
		freq_hex[i].tx_freq[2]=get_chr0(N); 
		freq_hex[i].tx_freq[3]=get_chr1(N); 
		freq_hex[i].tx_freq[4]=get_chr2(N);
		freq_hex[i].tx_freq[5]=L5; 
		freq_hex[i].tx_freq[6]=L6; 
		freq_hex[i].tx_freq[7]=L7;
	}
}

// here the pin change is calculated // 
ISR(PCINT0_vect) {
	unsigned char i=PORTB&(0x07);
	unsigned char ptt=(PORTB&(0x08))>>3;
	uint16_t ch=(PORTB&(0xF0))>>4;
	
	if ( ptt != 0x01 ) {
		PORTD=freq_hex[ch].rx_freq[i];
	} else {
		PORTD=freq_hex[ch].tx_freq[i];
	}
}



int main (void) {            

	DDRD=0b00001111; // PD0-3 are outbound 
	DDRB=0b00000000; // all pins inbound

	setup_channels();

	PCMSK1 |= (1<<PCINT0);
	sei(); 


	

   while(1) {                // (5)
     /* "leere" Schleife*/   // (6)
   }     
 
   /* wird nie erreicht */
   return 0;                 // (6)
}
