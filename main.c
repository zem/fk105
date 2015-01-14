#include <avr/io.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <avr/interrupt.h> 
#include <avr/sleep.h>
#include <avr/pgmspace.h>

//#define DEBUG 1

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
	{438675000, 431075000, 0},
	{438750000, 431150000, 0},
	{438725000, 430950000, 0},
	{438800000, 431200000, 0},
	{433500000, 433500000, 0},
	{0, 0, 0}
};
uint16_t num_channels=6; // this is to kick the setup_channel() initially


uint8_t freq [16];  // the current frequency is stored in those 16 Bytes

uint16_t channel=MAX_CHANNELS; // this is to kick the setup_channel() initially


uint16_t calc_N (uint32_t freq) {
	return (uint16_t) (freq/((uint32_t) fr * (uint32_t) M));
}

uint16_t calc_A (uint32_t freq, uint16_t N) {
	return (freq/fr) - M * N;
}

uint8_t get_chr0 (uint16_t n) { return (uint8_t) (n & 0x000F); }
uint8_t get_chr1 (uint16_t n) { return (uint8_t) ((n & 0x00F0)>>4); }
uint8_t get_chr2 (uint16_t n) { return (uint8_t) ((n & 0x0F00)>>8); }

#ifdef DEBUG
void setup_channel(uint16_t c) {
	uint16_t N;  
	uint16_t A;  
	if ( c >= num_channels ) { return; } // do nothing if c is 
								// higher than the last configured channel data
	// rx freq
	freq[0]=0x00;
	freq[1]=0x01;
	freq[2]=0x02; 
	freq[3]=0x03; 
	freq[4]=0x04;
	freq[5]=0x05; 
	freq[6]=0x06; 
	freq[7]=0x07;
	
	// tx freq
	freq[8]=0x08;
	freq[9]=0x09;
	freq[10]=0x0A; 
	freq[11]=0x0B; 
	freq[12]=0x0C;
	freq[13]=0x0D; 
	freq[14]=0x0E; 
	freq[15]=0x0F; 
	
	channel=c; 
}
#else
void setup_channel(uint16_t c) {
	uint16_t N;  
	uint16_t A;  
	if ( c >= num_channels ) { return; } // do nothing if c is 
								// higher than the last configured channel data
	// rx freq
	N=calc_N(freq_int[c].rx_freq-ZF);
	A=calc_A(freq_int[c].rx_freq-ZF, N);
	freq[0]=get_chr0(A);
	freq[1]=get_chr1(A);
	freq[2]=get_chr0(N); 
	freq[3]=get_chr1(N); 
	freq[4]=get_chr2(N);
	freq[5]=L5; 
	freq[6]=L6; 
	freq[7]=L7;
	
	// tx freq
	N=calc_N(freq_int[c].rx_freq);
	A=calc_A(freq_int[c].rx_freq, N);
	freq[8]=get_chr0(A);
	freq[9]=get_chr1(A);
	freq[10]=get_chr0(N); 
	freq[11]=get_chr1(N); 
	freq[12]=get_chr2(N);
	freq[13]=L5; 
	freq[14]=L6; 
	freq[15]=L7; 
	
	channel=c; 
}
#endif

// here the pin change is calculated // 
ISR(PCINT0_vect,ISR_NAKED) {
	PORTD=freq[(PINB>>1)];
	reti(); 
}


int main (void) {            
	uint8_t i;
	uint16_t ch;

	//DDRA=0b00000000;
	DDRB=0b00000000; // all pins inbound 4th bit is ptt. 
	DDRC=0b11111111; // outbound 
	DDRD=0b11111111; // PD0-3 are outbound 
	//PORTA=0b11111111; // all pins pull up
	//PORTB=0b11111111; // all pins pull up
	PORTB=0b00000000; // all pins ext pull down
	//PORTD=0b11111111; // all pins pull up

	// initialize num channels first... 
	//while (freq_int[num_channels].rx_freq > 0) { num_channels=num_channels+1; }
/*	PORTD=0xFF; 
	_delay_ms(1000);
	PORTD=0x00; */
	setup_channel(0); // anyway we start with channel code 0 to have 
						// something valid 
	//printf("hello world\n"); 
	
	PCMSK0 = 0x02;
	PCICR = 0x01;
	sei();

   while(1) {                // (5)
	//	ch=PINB; // pinreading takes time i guess
	//	i=ch&(0x07);
	//
	//	if ( ch&(0x08) == 0x08 ) { // ptt check 
	//		ch=(ch&(0xF0))>>4;
	//		if ( ch != channel ) { setup_channel(ch); } 
	//		PORTD=freq_hex.rx_freq[i];
	//	} else {
	//		ch=(ch&(0xF0))>>4;
	//		if ( ch != channel ) { setup_channel(ch); } 
	//		PORTD=freq_hex.tx_freq[i];
	//	}

		//sleep_bod_disable();
		//set_sleep_mode(SLEEP_MODE_STANDBY, SLEEP_MODE_EXT_STANDBY); 
   }     
 
   /* wird nie erreicht */
   return 0;                 // (6)
}
