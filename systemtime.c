#include "system.h"
 
unsigned char get_RTC_register(int reg) {
      outportb(0x70, reg);
      return inportb(0x71);
}
 
unsigned read_seconds() {
    return get_RTC_register(0x00);
}