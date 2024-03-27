#include <stdio.h>
#include "common.h"
#include "compress.h"
#include "uncompress.h"

int main(void)
{
    char input_file_name[MAX_FNAME+1];
    int operation;
    FILE *input, *output;
    
    printf("DIGITE A OPERACAO DESEJADA\n1-COMPACTAR\t2-DESCOMPACTAR\n");
    scanf("%d", &operation);
    
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    printf("DIGITE O NOME DO ARQUIVO:\n");
    fgets(input_file_name, MAX_FNAME, stdin);

    size_t len = strlen(input_file_name);
    if (len > 0 && input_file_name[len-1] == '\n') {
        input_file_name[len-1] = '\0';
    }

    input = fopen(input_file_name, "rb");
    output = create_output_file(input_file_name, operation);

    node *queue = NULL;

    switch (operation)
    {
        case COMPRESS:
            int bytes_freq[256] = {0}; 
            dictionary bytes_remap[256];
            for(int i = 0; i<256; i++)
            {
                bytes_remap[i].symbol_size = 0;
                bytes_remap[i].symbol = 0UL;
            }
            read_bytes(bytes_freq, input);
            fseek(input, 0, SEEK_SET); 
            create_priority_queue(&queue, bytes_freq);
            create_huffman_tree(&queue);
            remap(queue, bytes_remap, 0UL, 0, (8*sizeof(unsigned long)));
            compress(bytes_remap, queue, input, output);
            printf("\nO ARQUIVO \x1b[31m%S\x1b[0m FOI COMPACTADO COM SUCESSO\n", input_file_name);
            break;
        case UNCOMPRESS:
            int trash_size = 0, tree_size = 0;
            unsigned long file_size;
            get_file_size(input, &file_size);
            get_trash_tree_size(input, &trash_size, &tree_size);
            build_tree(&queue, input, tree_size);
            uncompress(queue, input, output, trash_size, (file_size-tree_size-2));
            printf("\nO ARQUIVO FOI DESCOMPACTADO COM SUCESSO\n", input_file_name);
            break;
    }
    free(queue);
    fclose(input);
    fclose(output);
}