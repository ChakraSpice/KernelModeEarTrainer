#include "system.h"

void play_sound(int nFrequence) {
 	unsigned short Div;
 	unsigned char tmp;
 
        //Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outportb(0x43, 0xb6);
 	outportb(0x42, (char) Div & 0xFF );
 	outportb(0x42, (char) ((Div >> 8)& 0xFF));
 
        //And play the sound using the PC speaker
 	tmp = inportb(0x61);
  	if (tmp != (tmp | 3)) {
 		outportb(0x61, tmp | 3);
 	}
 }
 
 //make it shutup
void nosound() {
 	char tmp = inportb(0x61) & 0xFC;
 
 	outportb(0x61, tmp);
 }
 
 //Make a beep
void beep() {
 	 play_sound(440);
 	 timer_wait(10);
 	 nosound();
	 play_sound(4000);
          //set_PIT_2(old_frequency);
 }