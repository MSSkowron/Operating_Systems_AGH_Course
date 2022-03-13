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

Block* createBlock()
{
    Block* tmp = malloc(sizeof(Block));
    tmp->words = 0;
    tmp->lines = 0;
    tmp->chars = 0;
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
    printf("index :%d -- lines: %d -- words: %d -- chars: %d\n",index,block->lines,block->words,block->chars);
}

void printArrayOfBlocks(BlocksArray* BlocksArray)
{
    printf("\n");
    printf("Printing ArrayOfBlocks\n");
    printf("Current size: %d\n",BlocksArray->curr_length);
    printf("Max size: %d\n",BlocksArray->max_size);
    printf("Blocks: \n");
    for(int i = 0;i<BlocksArray->curr_length;i++)
    {
        printBlock(BlocksArray,i);
    }
    printf("\n");
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
    Block* new_Block = createBlock();

    fscanf(file," %d %d %d",&new_Block->lines,&new_Block->words,&new_Block->chars);
    fclose(file);

    check_for_realloc(BlocksArray);
    BlocksArray->blocks[index] = new_Block;

    //remove tempfile
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