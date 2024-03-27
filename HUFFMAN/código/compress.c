#include "common.h"
#include "compress.h"

void enqueue(node **queue, node *left, node *right, unsigned char byte, int priority) //na fila de prioridade (left e right = NULL) enfileira no começo, na arvore (left e right != NULL) enfileira de acordo com a prioridade
{
    node *new_node = (node*) malloc(sizeof(node)); //aloca na memória espaço para new_node
    data *new_data = (data*) malloc(sizeof(data)); //aloca na memória espaço para new_data
    new_data->byte = byte; //new_data->byte recebe o parâmetro byte 
    new_data->frequency = priority; //new_data->frequency recebe o parametro prioridade
    new_node->data = new_data; //new_node->data aponta para new_data
    new_node->left = left; //new_node->left recebe o parametro left
    new_node->right = right; //new_node->right recebe o parametro right

    if(left == NULL && right == NULL) //a inserção está ocorrendo na criação fila de prioridade
    {
        new_node->next = *queue;
        *queue = new_node;
    }
    else //enfileirando durante a criaçao da árvore
    {
        node *current = *queue;
        node *previous = NULL;
        
        while (current != NULL && ((data*)current->data)->frequency < new_data->frequency)
        {
            previous = current;
            current = current->next;
        }
        
        new_node->next = current;

        if (previous == NULL)
        {
            // Se o novo nó for o primeiro da fila
            *queue = new_node;
        }
        else
        {
            // Se o novo nó for inserido no meio ou no fim da fila
            previous->next = new_node;
        }
    }
}

void read_bytes(int *bytes_freq, FILE *input) //função que percorre todo o arquivo de entrada para descobrir a frequencia de cada byte
{  
    while(1)
    {
        unsigned char byte;
        fread(&byte, 1, 1, input); // lê 1 byte do arquivo e armazena na variável byte
        if (feof(input)) return; // se o arquivo acabou sai do loop;
        int i = byte;
        bytes_freq[i] += 1; //soma 1 na frequencia do byte lido
    }
}

void create_priority_queue(node **queue, int *bytes_freq) //recebe como parametro o ponteiro duplo para a fila e a array de frequencia dos bytes
{
    int i, j;
    for(i=0; i<256; i++) //percorre a lista de frequencias 256 vezes (a quantidade de bytes possíveis)
    {
        int priority=0, byte_index;
        for(j=0; j<256; j++) //percorre o a lista de frequencia dos bytes procurando a maior frequencia
        {
            if(bytes_freq[j] >= priority && bytes_freq[j] > 0)
            {
                byte_index = j;
                priority = bytes_freq[j];
            }
        }

        if(priority > 0) 
        {
            bytes_freq[byte_index] = 0; //zera a frequencia na lista de frequencia
            unsigned char byte = byte_index; //byte recebe o byte que tem maior frequencia
            enqueue(queue, NULL, NULL, byte, priority); //enfileira o byte de maior frequencia
        }
    }
}

void create_huffman_tree(node **queue)
{
    while ((*queue)->next != NULL) //navega na fila para transformar ela em árvore
    {
        //pega dois elementos de menor prioridade
        int priority;
        node *left = *queue;
        *queue = (*queue)->next;
        node *right = *queue;
        *queue = (*queue)->next;
        
        struct data *left_data = (data*)left->data;
        struct data *right_data = (data*)right->data;

        priority = left_data->frequency + right_data->frequency; //a frequencia do nó pai será a soma da frequencia dos dois nós filhos
        enqueue(queue, left, right, '*', priority); //cria um nó pai com os filhos sendo os dois menores, remove eles da fila e enfileira o nó pai
    }
}
void write_pre_order_tree(node *root, FILE *output) //escreve a árvore em pré-ordem no arquivo de saída
{
    if (root != NULL) //funciona como a funçao padrão para imprimir em pré-ordem com a alteraçao para escrever o byte no arquivo de saída
    {
        if(root->left == NULL && root->right == NULL) 
        {
            unsigned char flag = 92;
            if (((data*)root->data)->byte == 42 || ((data*)root->data)->byte == 92)
            fwrite(&flag, 1, 1,output);
        }
        fwrite(&(((data*)root->data)->byte), 1, 1,output);
        write_pre_order_tree(root->left, output);
        write_pre_order_tree(root->right, output);
    }
}

void get_tree_size(node *root, int *size) //obtem o tamanho da árvore navegando por ela em pré ordem
{
    if (root != NULL) 
    {
        (*size)++;
        get_tree_size(root->left, size);
        get_tree_size(root->right, size);
    }
}

void int_to_byte(int tree_size, int trash_size, unsigned char trash_tree_size[2]) //converte de inteiro para os bytes que serão escritos pro header
{
    trash_tree_size[0] = '\0';
    trash_tree_size[1] = '\0';
  
    trash_tree_size[0] = trash_tree_size[0] | (trash_size << 5); //escreve nos tres primeiros bits do array o valor de trash_size

    trash_tree_size[0] = trash_tree_size[0] | (tree_size >> 8); //escreve nos bits restantes de trash_tree_size[0] os bits 12 a 8
    trash_tree_size[1] = trash_tree_size[1] | (tree_size); //escreve os bits de 
}

void compress(dictionary bytes_map[256], node *root, FILE *input, FILE *output)
{
    unsigned char new_byte = '\0', original_byte;
    short free_bits = 8, new_symbol_size=0;
    dictionary new_symbol;
    fwrite(&new_byte, 1, 2, output); //deixa dois bytes livres no começo de output para as informações acerca do tamanho do lixo e da árvore.
    write_pre_order_tree(root, output); //escreve os bytes da árvore em pré ordem
    
    //loop para compactar os arquivos com os novos simbolos    
    while(1) 
    {
        if(new_symbol_size == 0 && !(feof(input)))
        {
            fread(&original_byte, 1, 1, input); //lê um byte do arquivo de entrada e salva ele em original_byte
            new_symbol = bytes_map[original_byte];
            new_symbol_size = new_symbol.symbol_size;
        }

        if(!feof(input) && free_bits == 0) //quando esgota os bits livres de new_byte escreve ele no arquivo e reinicia para escrever o resto do new_symbol.byte
        {
            fwrite(&new_byte, 1, 1, output);
            new_byte = '\0';
            free_bits = 8;
        }
        else if(feof(input))
        {
            fwrite(&new_byte, 1, 1, output);
            break;
        }

        if(new_symbol.symbol & (1UL << (new_symbol_size-1)))
        {
            new_byte = set_bit(new_byte, free_bits-1);
        }

        free_bits--;
        new_symbol_size--;
    }
    
    unsigned char trash_tree_size[2] = {'\0'};
    int tree_size = 0;
    get_tree_size(root, &tree_size);
    int_to_byte(tree_size, free_bits, trash_tree_size);
    fseek(output, 0, SEEK_SET);
    fwrite(trash_tree_size, 1, 2, output);
}

void remap(node *root, dictionary bytes_map[256], unsigned long new_symbol, int i, int bit)
{
    if (root != NULL)
    {
        if (i == 1)
        {
            unsigned long mask = 1UL << bit;
            new_symbol = new_symbol | mask;
        }
        
        if(root->left == NULL && root->right == NULL)
        {
            int i = ((data*)root->data)->byte;
            bytes_map[i].symbol_size = (8*sizeof(unsigned long)) - bit;
            bytes_map[i].symbol = new_symbol >> bit;
        }
        remap(root->left, bytes_map, new_symbol, 0, bit-1);
        remap(root->right, bytes_map, new_symbol, 1, bit-1);
    }
}
