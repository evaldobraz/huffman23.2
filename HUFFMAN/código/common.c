#include "common.h"

unsigned char set_bit(unsigned char byte, int bit)
{
    unsigned char mask = 1<<bit;
    return byte | mask;
}

bool is_bit_set(unsigned char byte, int bit)
{
    unsigned char mask = 1<<bit;
    return byte & mask;
}

unsigned char left_shift(unsigned char byte, int shift_ammout)
{
    return byte << shift_ammout;
}

unsigned char right_shift(unsigned char byte, int shift_ammout)
{
    return byte >> shift_ammout;
}

FILE* create_output_file(char *file_name, short operation)
{
    FILE *output;
    char output_file_name[MAX_FNAME];
    switch (operation)
    {
        case COMPRESS:
            sprintf(output_file_name, "%s.huff", file_name);
            output = fopen(output_file_name, "wb");
            break;
        case UNCOMPRESS:
            int size = strlen(file_name) - 5;
            file_name[size] = '\0';
            sprintf(output_file_name, "%s", file_name);
            output = fopen(output_file_name, "wb");
            break;
    }
    return output;
}