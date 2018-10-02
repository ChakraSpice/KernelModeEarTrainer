#include "system.h"
#include <stdarg.h>

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,					/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

const int leftShift = 0x01;
const int rightShift = 0x02;
const int capsLock = 0x04;

volatile int keysMask = 0;

Interval intervals[9] = {
  { AUDIO_NOTE_A4, AUDIO_NOTE_A4, "u0" },
  { AUDIO_NOTE_C4, AUDIO_NOTE_E4, "M3" },
  { AUDIO_NOTE_A4, AUDIO_NOTE_E4, "p5" },
  { AUDIO_NOTE_A4, AUDIO_NOTE_A5, "o8" },
  { AUDIO_NOTE_A4, AUDIO_NOTE_B4, "M2" },
  { AUDIO_NOTE_A4, AUDIO_NOTE_D4, "p4" },
  { AUDIO_NOTE_E4, AUDIO_NOTE_F4, "m2" },
  { AUDIO_NOTE_A4, AUDIO_NOTE_F4, "M6" },
  { AUDIO_NOTE_A4, AUDIO_NOTE_G4, "m7" }
}; 

int gameRunning = 0;

int intervalIndex = 0;
int level = 0;
int maxLevel = 6;
int baseModulus = 3;
int playSound = 0;

int score = 0;
int numberOfTries = 0;
int currentLevelScore = 0;
const int nextLevelScore = 10;
const int wrongAnswerPenalty = 3;

int menuActive = 0;
int menuOptionIndex = -1;
int menuOptionMaxIndex = -1;

char* ops[3] = {
  "Option1\n\0",
  "Option2\n\0",
  "Option3\n\0"
  };

void activateMenu(int argsNumber, ...) {
  menuActive = 1;
  menuOptionIndex = 0;
  menuOptionMaxIndex = argsNumber - 1;
  
  va_list argsList;
  va_start(argsList, argsNumber);

  renderMenu(argsNumber, argsList);
  
  va_end(argsList);
}

void increaseMenuOptionIndex(int argsNumber, ...)
{
  if (!menuActive || menuOptionIndex == menuOptionMaxIndex)
    return;
  
  cls();
  va_list argsList;
  va_start(argsList, argsNumber);
  ++menuOptionIndex;
  renderMenu(argsNumber, argsList);
}

void decreaseMenuOptionIndex(int argsNumber, ...)
{
  if (!menuActive || menuOptionIndex == 0)
    return;

  cls();
  va_list argsList;
  va_start(argsList, argsNumber);
  --menuOptionIndex;
  renderMenu(argsNumber, argsList);
}

void renderMenu(int argsNumber, va_list argsList) 
{
  for(int i = 0; i < argsNumber; ++i) {
    char* textPtr = va_arg(argsList, char*);
    if (i == menuOptionIndex)
      putsColor(textPtr, WHITE, BLACK);
    else
      puts(textPtr);
  };
}

void disableMenu() {
  cls();
  menuActive = 0;
  menuOptionIndex = menuOptionMaxIndex = -1;
}

void newInterval() 
{
  intervalIndex = rand() % (baseModulus + level);
}

/* Handles the keyboard interrupt */
void keyboard_handler(struct regs *r)
{
    unsigned char scancode = inportb(0x60);
    char answer[3];

    if (scancode & 0x80)
    {
       switch (scancode) {
        // Left Shift
        case 170: 
          keysMask ^= leftShift;
          break;
        // Right shift
        case 182:
          keysMask ^= rightShift;
          break;
      }
    }
    else
    {
      switch (scancode) {
        // Escape key
        case 1: 
        disableMenu();
        // Enter key
        case 28:
        if (menuActive) 
        {
          int picked = menuOptionIndex;
          int offset = 0;
          disableMenu();
          puts(ops[picked]);
        }

        if (gameRunning) {
          memcpy(answer, getCurrentLine(), 3);

          if (!strcmp(answer, intervals[intervalIndex].answer))
          {
            puts("\nGreat success!\n");
            playSound = 1;
            ++score;
            ++numberOfTries;
            if (++currentLevelScore == nextLevelScore) 
            {
              currentLevelScore = 0;
              if (level < maxLevel) 
              {
                ++level;
                puts("Level up!\n");
              }
            }
            newInterval(); 
          }
          else {
            ++numberOfTries;
            if (currentLevelScore >= wrongAnswerPenalty)
              currentLevelScore -= wrongAnswerPenalty;
            puts("\nWrong. Try again. If you wish to play the interval again, press \"a\", if you wish to quit, press \"q\".\n");
          } 
        } 
        else 
        {
          if (!strcmp("Start game", getCurrentLine())) {
            puts("\nGame started! \n To quit, press \"q\".\n To play interval again, press \"a\".\n");
            puts("Interval codes are:\n");
            puts("Unison: u0\n");
            puts("Minor second: m2\n");
            puts("Major second: M2\n");
            puts("Minor third: m3\n");
            puts("Major third: M3\n");
            puts("Perfect fourth: p4\n");
            puts("Tritone: tr\n");
            puts("Perfect fifth: p5\n");
            puts("Octave: o8\n");
            gameRunning = 1; 
            playSound = 1;
            score = numberOfTries = level = currentLevelScore = 0;
          }
        }
        break;

        // Left Shift
        case 42: 
          keysMask |= leftShift;
          break;

        // Right shift
        case 54:
          keysMask |= rightShift;
          break;

        // Caps Lock
        case 58:
          keysMask ^= capsLock;
          break;

        // Up arrow
        case 72:
          decreaseMenuOptionIndex(3, "Option1\n", "Option2\n", "Option3\n");
          break;

        // Left arrow
        case 75:
          break;

        // Right arrow
        case 77:
          cls();
          activateMenu(3, "Option1\n", "Option2\n", "Option3\n");
          break;
        
        // Down arrow
        case 80:
          increaseMenuOptionIndex(3, "Option1\n", "Option2\n", "Option3\n");
          break;
      }

      putch(kbdus[scancode]);
      // Play again 
      if (gameRunning) {
        if (kbdus[scancode] == 'a') {
          playSound = 1;
          puts("\n");
        }
        if (kbdus[scancode] == 'q') {
          gameRunning = 0;
          puts("\nScore \\ Number of tries: "); printNumber(score); puts(" \\ "); printNumber(numberOfTries); puts("\n");
          puts("Level: "); printNumber(level); puts("\n");
          puts("To start the game again, type in \"Start game\".");
        }
      }
      if (playSound)
      {
        playSound = 0;  
        playInterval(intervals[intervalIndex]);
      }
    }
}

void keyboard_install()
{
    irq_install_handler(1, keyboard_handler);
}