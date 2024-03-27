#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_FNAME 256
#define COMPRESS 1
#define UNCOMPRESS 2
#define PO_TREE_SCAPE 92 //símbolo de escape da impressao em pré-ordem dá árvore

typedef struct map dictionary;
typedef struct node node;
typedef struct data data;

struct node{
    void *data;
    node *next;
    node *left;
    node *right;
};

struct data{
    unsigned char byte;
    long frequency;
};

struct map
{
    short symbol_size;
    unsigned long symbol;
};

unsigned char set_bit(unsigned char byte, int bit);
unsigned char unset_bit(unsigned char byte, int bit);
bool is_bit_set(unsigned char byte, int bit);
unsigned char left_shift(unsigned char byte, int shift_ammout);
unsigned char right_shift(unsigned char byte, int shift_ammout);
FILE* create_output_file(char *file_name, short operation);

#endif //COMMON_H_INCLUDED