#include "system.h"

void play_sound(int nFrequence) {
 	unsigned short Div;
 	unsigned char tmp;
 
        //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outportb(0x43, 0xb6); // Command 
 	outportb(0x42, (char) Div & 0xFF );
 	outportb(0x42, (char) ((Div >> 8)& 0xFF));
 
        //And play the sound using the PC speaker
 	tmp = inportb(0x61);
  	if (tmp != (tmp | 3))
 		outportb(0x61, tmp | 3); // What the fuck does this line do? 
 }
 

const int A = 440;
const int C = 523;
const int E = 659;
 //make it shutup
void nosound() {
 	unsigned char tmp = inportb(0x61) & 0xFC;
 
 	outportb(0x61, tmp);
 }
 
 //Make a beep
void beep() {
	play_sound(440);
	timer_wait(200);
	nosound();
 }