/* bkerndev - Bran's Kernel Development Tutorial
*  By:   Brandon F. (friesenb@gmail.com)
*  Desc: Timer driver
*
*  Notes: No warranty expressed or implied. Use at own risk. */
#include "system.h"

/* This will keep track of how many ticks that the system
*  has been running for */
volatile unsigned int timer_ticks = 0;
int timer_freq = 18; // Default, can change 
// Sets the phase of the timer. 

int _freq_to_play[10];
int _play_length[10];
int originalPauseTime;
int currentPauseTime;
volatile int freq_index = -1;
volatile int numberOfFreqs = 0;

void timer_phase(int hz)
{
    timer_freq = hz;
    int divisor = 1193180 / timer_freq;       /* Calculate our divisor */
    outportb(0x43, 0x36);             /* Set our command byte 0x36 */
    outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outportb(0x40, divisor >> 8);     /* Set high byte of divisor */

    // TODO: Set 0x42 channel for channel 2 PIT 
}

/* Handles the timer. In this case, it's very simple: We
*  increment the 'timer_ticks' variable every time the
*  timer fires. By default, the timer fires 18.222 times
*  per second. Why 18.222Hz? Some engineer at IBM must've
*  been smoking something funky */
void timer_handler(struct regs *r)
{
    /* Increment our 'tick count' */
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

/* This will continuously loop until the given time has
*  been reached */
void timer_wait(int ticks)
{
    puts("Timer wait called");
    unsigned long eticks;

    // Keyboard handler determines what will be played
    // PIT handler playes what was determined in the keyboard handler 

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
    puts("Waited for ");
    printNumber(ticks);
    puts("\n");
}

/* Sets up the system clock by installing the timer handler
*  into IRQ0 */
void timer_install(int hz)
{
    timer_phase(hz);
    /* Installs 'timer_handler' to IRQ0 */
    irq_install_handler(0, timer_handler);
}

void playFreqForTime(int freqsToPlay[], int lengthsToPlay[], int arrLength, int pauseTime) 
{
    unsigned int size = sizeof(int)*arrLength;
    memcpy(_freq_to_play, freqsToPlay, size);
    memcpy(_play_length, lengthsToPlay, size);
    numberOfFreqs = arrLength;
    currentPauseTime = originalPauseTime = pauseTime;
    freq_index = 0; // Rudimentary locking mechanism :D 
}