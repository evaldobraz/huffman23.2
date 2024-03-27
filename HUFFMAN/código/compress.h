#ifndef COMPRESS_H_INCLUDED
#define COMPRESS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "common.h"


void enqueue(node **queue, node *left, node *right, unsigned char byte, int priority);
void read_bytes(int *bytes_freq, FILE *input); 
void create_priority_queue(node **queue, int *bytes_freq);
void create_huffman_tree(node **queue);
void write_pre_order_tree(node *root, FILE *output);
void get_tree_size(node *root, int *size);
void int_to_byte(int tree_size, int trash_size, unsigned char trash_tree_size[2]);
void compress(dictionary bytes_remap[256], node *root, FILE *input, FILE *ouput);
void remap(node *root, dictionary bytes_map[256], unsigned long new_symbol, int i, int bit);

#endif //COMPRESS_H_INCLUDED