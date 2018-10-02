#ifndef __SYSTEM_H
#define __SYSTEM_H

typedef int size_t;

struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;    
};

typedef enum {
    AUDIO_NOTE_A4 = 440,
    AUDIO_NOTE_B4 = 494,
    AUDIO_NOTE_C4 = 523,
    AUDIO_NOTE_D4 = 587,
    AUDIO_NOTE_E4 = 659,
    AUDIO_NOTE_F4 = 698,
    AUDIO_NOTE_G4 = 784,
    AUDIO_NOTE_A5 = 880
} AUDIO_NOTE;

typedef struct {
    AUDIO_NOTE note1; // Lower note
    AUDIO_NOTE note2; // Higher note 
    char answer[3];
} Interval;

typedef enum {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHT_GREY,
    DARK_GREY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_MAGENTA,
    LIGHT_BROWN,
    WHITE
} COLOR;

/* MAIN.C */
extern void *memcpy(void *dest, const void *src, size_t count);
extern void *memset(void *dest, char val, size_t count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
extern size_t strlen(const char *str);
extern int strcmp(const char *s1, const char *s2);
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);

extern int rand(void);
extern void srand(unsigned int seed);

/* CONSOLE.C */
extern void init_video(void);
extern void puts(unsigned char *text);
extern void putsColor(unsigned char *text, COLOR backgroudColor, COLOR textColor);
extern void putch(unsigned char c);
extern void putchColor(unsigned char c, COLOR backgroudColor, COLOR textColor);
extern void cls();
extern void printNumber(int number);
extern char* getCurrentLine();

/* GDT.C */
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_install();

/* IDT.C */
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();

/* ISRS.C */
extern void isrs_install();

/* IRQ.C */
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern void irq_install();

/* TIMER.C */
extern void timer_wait(int ticks);
extern void timer_install(int hz);
extern void timer_phase(int hz);
extern void setBeep();

/* KEYBOARD.C */
extern void keyboard_install();
extern void newInterval();

/* PCSPEAKER.C */
extern void playFreqsForTime(int* freqsToPlay, int* lengthsToPlay, int arrLength, int pauseTime);
extern void nosound();
extern void play_sound(int nFrequence);
extern void beep();
extern void playInterval(Interval intrvl);

extern volatile int keysMask;

/* SYSTEMTIME.C */
extern unsigned char get_RTC_register(int reg);
extern unsigned read_seconds();

#endif