#include <avr/io.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <avr/interrupt.h> 
#include <avr/sleep.h>

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
#define L5 0x0C
#define L6 0x03
#define L7 0x00


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
	uint8_t rx_freq[8];
	uint8_t tx_freq[8]; 
};
 
struct freq_hex_t freq_hex[MAX_CHANNELS+1];
int anz_channels=0; 


uint16_t calc_N (uint32_t freq) {
	return (uint16_t) (freq/((uint32_t) fr * (uint32_t) M));
}

uint16_t calc_A (uint32_t freq, uint16_t N) {
	return (freq/fr) - M * N;
}

uint8_t get_chr0 (uint16_t n) { return (uint8_t) (n & 0x000F); }
uint8_t get_chr1 (uint16_t n) { return (uint8_t) ((n & 0x00F0)>>1); }
uint8_t get_chr2 (uint16_t n) { return (uint8_t) ((n & 0x0F00)>>2); }

void setup_channels() {
	uint8_t i=0;
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
		//PORTD=freq_hex[i].rx_freq[0];
		//_delay_ms(1000); 
		freq_hex[i].rx_freq[1]=get_chr1(A);
		//PORTD=freq_hex[i].rx_freq[1];
		//_delay_ms(1000); 
		freq_hex[i].rx_freq[2]=get_chr0(N); 
		//PORTD=freq_hex[i].rx_freq[2];
		//_delay_ms(1000); 
		freq_hex[i].rx_freq[3]=get_chr1(N); 
		//PORTD=freq_hex[i].rx_freq[3];
		//_delay_ms(1000); 
		freq_hex[i].rx_freq[4]=get_chr2(N);
		//PORTD=freq_hex[i].rx_freq[4];
		//_delay_ms(1000); 
		freq_hex[i].rx_freq[5]=L5; 
		//PORTD=freq_hex[i].rx_freq[5];
		//_delay_ms(1000); 
		freq_hex[i].rx_freq[6]=L6; 
		//PORTD=freq_hex[i].rx_freq[6];
		//_delay_ms(1000); 
		freq_hex[i].rx_freq[7]=L7;
		//PORTD=freq_hex[i].rx_freq[7];
		//_delay_ms(1000); 
		PORTD=0x0F;
		_delay_ms(100); 
		PORTD=0x00;
		_delay_ms(100); 
		PORTD=0x0F;
		_delay_ms(100); 
		
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
	uint8_t i=PINB&(0x07);
	uint8_t ptt=(PINB&(0x08))>>3;
	uint16_t ch=(PINB&(0xF0))>>4;
	
	if ( ptt != 0x01 ) {
		PORTD=freq_hex[ch].rx_freq[i];
	} else {
		PORTD=freq_hex[ch].tx_freq[i];
	}
}


int main (void) {            
	//unsigned char i=PORTB&(0x07);
	//unsigned char ptt=(PORTB&(0x08))>>3;
	//uint16_t ch=(PORTB&(0xF0))>>4;
	uint8_t on=3; 
	unsigned char foo; 

	//DDRA=0b00000000;
	DDRB=0b00000000; // all pins inbound 5th bit is ptt. 
	DDRC=0b11111111; // outbound 
	DDRD=0b11111111; // PD0-3 are outbound 
	//PORTA=0b11111111; // all pins pull up
	PORTB=0b11111111; // all pins pull up
	//PORTD=0b11111111; // all pins pull up

	setup_channels();
	
	//PORTD=freq_hex[1].tx_freq[2];

//	PCICR |= (1<<PCIE1);
//	PCMSK1 |= (1<<PCINT0);
//	sei(); 

   while(1) {                // (5)
		//i=(unsigned char) PORTB&((1<<PB0)|(1<<PB1)|(1<<PB2));
		//ptt=(unsigned char) (PORTB&(1<<PB3))>>3;
		//ch=(uint16_t) (PORTB&((1<<PB4)|(1<<PB5)|(1<<PB6)|(1<<PB7)))>>4;
		

//			if ( on == 0 ) { PORTD=freq_hex[0].rx_freq[0]; }
//			if ( on == 1 ) { PORTD=freq_hex[0].rx_freq[1]; }
//			if ( on == 2 ) { PORTD=freq_hex[0].rx_freq[2]; }
//			if ( on == 3 ) { PORTD=freq_hex[0].rx_freq[3]; }
//			if ( on == 4 ) { PORTD=freq_hex[0].rx_freq[4]; }
//			if ( on == 5 ) { PORTD=freq_hex[0].rx_freq[5]; }
//			if ( on == 6 ) { PORTD=freq_hex[0].rx_freq[6]; }
//			if ( on == 7 ) { PORTD=freq_hex[0].rx_freq[7]; }
//			on=on+1; if ( on>=8 ) { on=0; }
			 
			PORTD=freq_hex[(uint8_t) 0].rx_freq[0]>>4; 
			_delay_ms(1000);
			PORTD=0xFF; 
			_delay_ms(100);

			PORTD=freq_hex[(uint8_t) 1].rx_freq[1]>>4; 
			_delay_ms(1000);
			PORTD=0xFF; 
			_delay_ms(100);

			PORTD=freq_hex[(uint8_t) 0].rx_freq[2]>>4; 
			_delay_ms(1000);
			PORTD=0xFF; 
			_delay_ms(100);

			PORTD=freq_hex[(uint8_t) 1].rx_freq[3]; 
			_delay_ms(1000);
			PORTD=0xFF; 
			_delay_ms(100);

			PORTD=freq_hex[(uint8_t) 0].rx_freq[4]; 
			_delay_ms(1000);
			PORTD=0xFF; 
			_delay_ms(100);

			PORTD=freq_hex[(uint8_t) 1].rx_freq[5]; 
			_delay_ms(1000);
			PORTD=0xFF; 
			_delay_ms(100);

			PORTD=freq_hex[(uint8_t) 0].rx_freq[6]; 
			_delay_ms(1000);
			PORTD=0xFF; 
			_delay_ms(100);

			PORTD=freq_hex[(uint8_t) 1].rx_freq[7]; 
			_delay_ms(1000);
			PORTD=0xFF; 
			_delay_ms(100);


			//PORTD=on; 


//			if ( on == 0 ) {
//				on=1; 
//				PORTD=freq_hex[0].tx_freq[PINB&0x0F]; 
//				DDRB=0b00000000; // all pins inbound
//				PORTB=0b11111111; // all pins pull up
//				//PORTD=(1<<PD0);
//			} else {
//				on=0; 
//				PORTD=(0x00);
//			}



//	if ( ptt != 0x01 ) {
//			PORTD=freq_hex[ch].rx_freq[i];
	//		//PORTD=0x0f;
//		} else {
//			PORTD=freq_hex[ch].tx_freq[i];
//		}
//		sei(); 
   }     
 
   /* wird nie erreicht */
   return 0;                 // (6)
}
