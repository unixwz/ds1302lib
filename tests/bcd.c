#include <stdio.h>

void print_bin(char value)
{
    for (int i = 7; i >= 0; --i) {
        if (value & (1 << i))
            printf("%d", 1);
        else
            printf("%d", 0);
    }
    printf("\n");
}

int main()
{
    char num = 0b00010000;
    for (int i = 0; i < 8; ++i) {
        num = num << 1;
        num += 0b11;
        print_bin(num);
    }
    return 0;
}