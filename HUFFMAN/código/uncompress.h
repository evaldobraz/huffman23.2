#ifndef UNCOMPRESS_H_INCLUDED
#define UNCOMPRESS_H_INCLUDED
#include "common.h"

    void build_tree(node **root, FILE *input, int tree_size);
    void get_trash_tree_size(FILE *input, int *trash_size, int *tree_size);
    void uncompress(node *root, FILE *input, FILE *output, int trash_size, unsigned long file_size);
    void get_file_size(FILE *input, unsigned long *file_size);

#endif //UNCOMPRESS_H_INCLUDED