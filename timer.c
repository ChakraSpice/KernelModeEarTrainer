#include "system.h"

volatile unsigned int timer_ticks = 0;
int timer_freq = 18; // Default, can change 

int _freq_to_play[10];
int _play_length[10];
int originalPauseTime;
int currentPauseTime;
volatile int freq_index = -1;
volatile int numberOfFreqs = 0;

void timer_phase(int hz)
{
    timer_freq = hz;
    int divisor = 1193180 / timer_freq;
    outportb(0x43, 0x36);             
    outportb(0x40, divisor & 0xFF);   
    outportb(0x40, divisor >> 8);     
}

void timer_handler(struct regs *r)
{
    timer_ticks++;

    if (freq_index >= 0) {
        if(_play_length[freq_index] > 0) {
            _play_length[freq_index]-=timer_freq;
            play_sound(_freq_to_play[freq_index]);
        } else {
            nosound();
            if(currentPauseTime > 0)
            {
                currentPauseTime-=timer_freq;
            }
            else 
            {
                ++freq_index;
                currentPauseTime = originalPauseTime;
                if (freq_index == numberOfFreqs)
                    freq_index = -1;
            }
        }
    }
}

void timer_wait(int ticks)
{
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks)
	    __asm__ __volatile__("hlt");
}

void timer_install(int hz)
{
    timer_phase(hz);
    irq_install_handler(0, timer_handler);
}

void playFreqsForTime(int freqsToPlay[], int lengthsToPlay[], int arrLength, int pauseTime) 
{
    unsigned int size = sizeof(int)*arrLength;
    memcpy(_freq_to_play, freqsToPlay, size);
    memcpy(_play_length, lengthsToPlay, size);
    numberOfFreqs = arrLength;
    currentPauseTime = originalPauseTime = pauseTime;
    freq_index = 0; // Rudimentary locking mechanism :D 
}

void playInterval(Interval intrvl) 
{
    int frqs[2] = { intrvl.note1, intrvl.note2 };
    int lngths[2] = { 3500, 3500 }; 
    playFreqsForTime(frqs, lngths, 2, 1000);
}