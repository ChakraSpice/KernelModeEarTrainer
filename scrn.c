#include "system.h"

unsigned short *textmemptr;
int csr_x = 0, csr_y = 0;

const int width = 80;
const int height = 25;
char currentLine[80];

unsigned blank = 0x20 | (BLACK << 8);

void scroll(void)
{
    unsigned temp;

    if(csr_y >= height)
    {
        temp = csr_y - height + 1;
        memcpy (textmemptr, textmemptr + temp * width, (height - temp) * width * 2);

        memsetw (textmemptr + (height - temp) * width, blank, width);
        csr_y = height - 1;
    }
}

void move_csr(void)
{
    unsigned temp = csr_y * width + csr_x;

    outportb(0x3D4, 14);
    outportb(0x3D5, temp >> 8);
    outportb(0x3D4, 15);
    outportb(0x3D5, temp);
}

void cls()
{
    for(int i = 0; i < height; i++)
        memsetw (textmemptr + i * width, blank, width);
    
    csr_x = 0;
    csr_y = 0;
    move_csr();
}

void putch(unsigned char c)
{
    putchColor(c, BLACK, WHITE);
}

void putchColor(unsigned char c, COLOR backgroudColor, COLOR textColor) 
{
    unsigned short *where;
    unsigned short attribute = textColor;
    attribute |= backgroudColor << 4;

    if(c == 0x08 && csr_x != 0)
    {
        --csr_x;
    }
    else if(c == 0x09)
    {
        csr_x = (csr_x + 8) & ~(8 - 1);
    }
    else if(c == '\r')
    {
        csr_x = 0;
    }
    else if(c == '\n')
    {
        csr_x = 0;
        csr_y++;
    }
    else if(c >= ' ')
    {
        // Capital letters, handling shifts too 
        if (c >= 'a' && c <= 'z')
        { 
            if (keysMask & 0x04)
            {
                if(!(keysMask & 0x02 || keysMask & 0x01))
                    c-= 32;
            }
            else if (keysMask & 0x02 || keysMask & 0x01)
                c-= 32;
        }
        where = textmemptr + (csr_y * width + csr_x);
        *where = c | (attribute << 8);	/* Character AND attributes: color */
        csr_x++;
    }

    if(csr_x >= width)
    {
        csr_x = 0;
        csr_y++;
    }

    scroll();
    move_csr();
}

void puts(unsigned char *text)
{
    for (int i = 0; i < strlen(text); ++i)
    {
        putch(text[i]);
    }
}

void putsColor(unsigned char *text, COLOR backgroudColor, COLOR textColor) 
{
    for (int i = 0; i < strlen(text); ++i)
    {
        putchColor(text[i], backgroudColor, textColor);
    }
}

void printNumber(int number) 
{
    char isNegative = number < 0;

    if (isNegative)
        number = - number;

    int numberCopy = number;
    int digitNumber = 0;
    
    while (numberCopy != 0)
    {
        numberCopy /= 10;
        ++digitNumber;
    }

    if (digitNumber == 0)
    {    
        putch('0');
        return;
    }

    unsigned char digits[20];

    if (!isNegative)
        --digitNumber;
    else 
        digits[0] = '-';

    digits[digitNumber + 1] = '\0';

    numberCopy = number;

    while (numberCopy != 0) 
    {
        int digit = numberCopy % 10;
        numberCopy /= 10;
        digits[digitNumber--] = '0' + digit;
    }

    puts(digits);
}

char* getCurrentLine()
{   
    int coord_x = csr_x;
    int lineBeginning = csr_y * width;
    for (int i = 0; i < coord_x; ++i)
        currentLine[i] = (char)((textmemptr + lineBeginning)[i]);
    currentLine[coord_x] = '\0';

    return currentLine;
}

void init_video(void)
{
    textmemptr = (unsigned short *)0xB8000;
    cls();
}