#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "mylib.h"

BlocksArray* createArrayOfBlocks(int size)
{
    if(size < 0)
    {
        return NULL;
    }

    BlocksArray* p_BlocksArray = calloc(1,sizeof(BlocksArray));
    p_BlocksArray->curr_length = 0;
    p_BlocksArray->max_size = size;
    p_BlocksArray->blocks = (Block**) calloc(size,sizeof(Block*));

    return p_BlocksArray;
}

int removeBlock(BlocksArray* BlocksArray,int index)
{
    if (index < 0 || index >= BlocksArray->curr_length)
    {
        return -1;
    }

    free(BlocksArray->blocks[index]->wc_result);
    free(BlocksArray->blocks[index]->fileName);
    free(BlocksArray->blocks[index]);

    int i = index + 1;
    while(i < BlocksArray->curr_length)
    {
        BlocksArray->blocks[i-1] = BlocksArray->blocks[i];
        BlocksArray->blocks[i] = NULL;
        i += 1;
    }

    BlocksArray->curr_length--;
    return 1;
}

Block* createBlock(int fileSize,int fileNameSize)
{
    Block* tmp = malloc(sizeof(Block));
    tmp->wc_result = malloc(fileSize);
    tmp->fileName = malloc(fileNameSize);
    return tmp;
}

void removeArrayOfBlocks(BlocksArray* BlocksArray)
{
    for(int i = 0;i < BlocksArray->curr_length; i++)
    {
        removeBlock(BlocksArray,i);
    }
    free(BlocksArray->blocks);
    free(BlocksArray);
}

void printBlock(BlocksArray* BlocksArray,int index)
{
    Block* block = BlocksArray->blocks[index];
    printf("Block index: %d\n",index);
    printf("file: %s\n",block->fileName);
    printf("wc_result: %s\n",block->wc_result);
}

void printArrayOfBlocks(BlocksArray* BlocksArray)
{
    printf("Printing ArrayOfBlocks\n");
    printf("Current size: %d\n",BlocksArray->curr_length);
    printf("Max size: %d\n",BlocksArray->max_size);
    printf("Blocks: \n");
    printf("\n");
    for(int i = 0;i<BlocksArray->curr_length;i++)
    {
        printBlock(BlocksArray,i);
    }
}

void run_wc(const char* from,const char* to)
{
    char command[100];

    strcpy(command,"wc -w -l -m < ");
    strcat(command,from);
    strcat(command," >> ");
    strcat(command,to);

    system(command);
}

void check_for_realloc(BlocksArray* BlocksArray)
{
    if(BlocksArray->curr_length >= BlocksArray->max_size)
    {
        BlocksArray->max_size *= 2;
        BlocksArray->blocks = realloc(BlocksArray->blocks,BlocksArray->max_size * sizeof(Block*));
    }
}

int get_new_index(BlocksArray* BlocksArray)
{
    BlocksArray->curr_length++;
    return BlocksArray->curr_length - 1;
}

int count(BlocksArray* BlocksArray,const char* from,const char* to)
{
    run_wc(from,to);

    FILE* file = fopen(to,"r");

    if(!file)
    {
        return -1;
    }

    int index = get_new_index(BlocksArray);

    fseek(file,0,SEEK_END);
    int f_size = ftell(file);
    fseek(file,0,SEEK_SET);

    Block* new_Block = createBlock(f_size, strlen(to) + 1);

    strcpy(new_Block->fileName,from);

    size_t n = 0;
    int c;
    while((c = fgetc(file))!= EOF)
    {
        new_Block->wc_result[n++] = (char)c;
    }
    new_Block->wc_result[n] = '\0';

    fclose(file);

    check_for_realloc(BlocksArray);
    BlocksArray->blocks[index] = new_Block;

    fclose(fopen(to,"w"));

    return index;
}

int is_number(char* str)
{
    int len = strlen(str);
    for(int i = 0; i < len; i++)
    {
        if(!isdigit(str[i]))
        {
            return -1;
        }
    }
    return 1;
}

void create_temp_file()
{
    system("touch temp.txt");
}

void delete_temp_file()
{
    system("rm -f temp.txt");
}