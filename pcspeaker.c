#include "system.h"

void play_sound(int nFrequence) 
{
 	unsigned short div;
 	unsigned char tmp;
 
	div = 1193180 / nFrequence;
 	outportb(0x43, 0xb6); // Command 
 	outportb(0x42, (char) div & 0xFF );
 	outportb(0x42, (char) ((div >> 8)& 0xFF));

 	tmp = inportb(0x61);
  	if (tmp != (tmp | 3))
 		outportb(0x61, tmp | 3);
 }
 
void nosound() 
{
	unsigned char tmp = inportb(0x61) & 0xFC;
	outportb(0x61, tmp);
}
 
void beep() 
{
	play_sound(AUDIO_NOTE_C4);
	timer_wait(1000);
	nosound();
}