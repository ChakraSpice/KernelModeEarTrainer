#include "system.h"

void *memcpy(void *dest, const void *src, size_t count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

void *memset(void *dest, char val, size_t count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

size_t strlen(const char *str)
{
    size_t retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

unsigned char inportb (unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb (unsigned short _port, unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

volatile unsigned long int next = 1;

int rand(void) 
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}
// TODO: SEED 
void srand(unsigned int seed)
{
    next = seed;
}

void main()
{
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    init_video();
    // Set the freq
    timer_install(100);
    keyboard_install();

    __asm__ __volatile__ ("sti");

    puts("Welcome to your personal ear trainer!\n");
    srand(read_seconds());
    newInterval();
    puts("\nTo start the game type in \"Start game\"\n.");
    for (;;);
}
