#include "kmath.h"
#include "klib.h"

void kdecimalToHexadecimal(char *hexadecimalNum, int decimalNum)
{
    int i = 0;

    if (decimalNum == 0)
    {
        hexadecimalNum[0] = '0';
        hexadecimalNum[1] = '\0';
        return;
    }

    while (decimalNum > 0)
    {
        int remainder = decimalNum % 16;

        if (remainder < 10)
            hexadecimalNum[i] = remainder + '0'; // Convert to ASCII character 0-9
        else
            hexadecimalNum[i] = remainder + 55; // Convert to ASCII character A-F

        i++;
        decimalNum /= 16;
    }
    hexadecimalNum[i] = 0;
    /* 倒置顺序 */
    kreverse_charstr(hexadecimalNum);
}