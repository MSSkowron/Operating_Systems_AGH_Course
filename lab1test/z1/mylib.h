#ifndef MYLIB_H_
#define MYLIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Block{
    int lines;
    int words;
    int chars;
} Block;

typedef struct BlocksArray{
    Block** blocks;
    int curr_length;
    int max_size;
}BlocksArray;

BlocksArray* createArrayOfBlocks(int size);
int removeBlock(BlocksArray* BlocksArray,int index);
Block* createBlock();
void removeArrayOfBlocks(BlocksArray* BlocksArray);
void printBlock(BlocksArray* BlocksArray,int index);
void printArrayOfBlocks(BlocksArray* BlocksArray);
void run_wc(const char* from,const char* to);
void check_for_realloc(BlocksArray* BlocksArray);
int get_new_index(BlocksArray* BlocksArray);
int count(BlocksArray* BlocksArray,const char* r_file_path,const char* t_file_path);
int is_number(char* str);
void create_temp_file();
void delete_temp_file();
#endif