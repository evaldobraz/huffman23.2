#include "common.h"
#include "uncompress.h"

void build_tree(node **root, FILE *input, int tree_size)
{
    if (tree_size <= 0) return; 
    tree_size--;

    unsigned char byte;
    int n = fread(&byte, 1, 1, input);

    if (byte == PO_TREE_SCAPE) n += fread(&byte, 1, 1, input); //PO_TREE_SCAPE = BYTE 5C

    node *new_node = (node*) malloc(sizeof(node));
    data *new_data = (data*) malloc(sizeof(data));
    new_data->byte = byte;
    new_data->frequency = 0;
    new_node->data = new_data;
    new_node->left = NULL;
    new_node->right = NULL;

    if (byte == '*' && n == 1) //nó interno
    {
        build_tree(&(new_node->left), input, tree_size);
        build_tree(&(new_node->right), input, tree_size);
    }

    *root = new_node;
}

void get_trash_tree_size(FILE *input, int *trash_size, int *tree_size)
{
    unsigned char trash_tree_size[2] = {'\0'};
    fread(trash_tree_size, 1, 2, input);
    
    *trash_size =  (trash_tree_size[0] >> 5) & 7;
    *tree_size =  ((trash_tree_size[0] & 31) << 8) | trash_tree_size[1];
}

void uncompress(node *root, FILE *input, FILE *output, int trash_size, unsigned long file_size)
{
    unsigned long read_bytes = 1UL;
    int bit = 7, is_last_byte = 0;
    unsigned char byte;
    fread(&byte, 1, 1, input);
    node *temp_root = root;

    while(1)
    {
        temp_root = root;

        while(temp_root->left != NULL && temp_root->right != NULL)
        {
            if(is_bit_set(byte, bit)) temp_root = temp_root->right;
            else temp_root = temp_root->left;
            bit--;

            if(bit < 0)
            {
                bit = 7;
                fread(&byte, 1, 1, input);
                read_bytes++;
                is_last_byte = (read_bytes == file_size);
            }
        }
        fwrite((&((data*)temp_root->data)->byte), 1, 1, output);
        
        if(is_last_byte && bit <= trash_size) break;
    }
}

void get_file_size(FILE *input, unsigned long *file_size)
{
    fseek(input, 0, SEEK_END);

    *file_size = ftell(input);

    fseek(input, 0, SEEK_SET);
}