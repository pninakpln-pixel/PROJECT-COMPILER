#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "table.h"
#include "globals.h"
#include "helpers.h"

/*
* Make the complete first pass of the assembler for the input source file. Coding instructions while filling the symbol table and memory images.
* Inputs: 
* - file_name - name of assembly source file to process.
* - symbol_head - double pointer to head of the symbol table linked list.
* - code_img - pointer to code image memory array.
* - data_img - double pointer to data image memory array.
* Output: SUCCESS_F (1) if first pass completed without errors, ERROR_F (0) else.
*/
int first_pass(char *file_name, Symbol **symbol_head, unsigned char *code_img, char **data_img);

/**
* Reads numeric arguments from a data directive line, validates comma's place, and stores them in an array.
* Inputs: 
* - str - pointer to the argument string of the data directive.
* - values_out - array to store extracted values.
* - count_out - pointer to store the count of values extracted.
* - line_number - current line number in source file.
* - file_name - name of the processed file.
* Output: SUCCESS_F (1) if syntax is valid, ERROR_F (0) on missing arguments or comma errors.
*/
int extract_numbers_data(char *str, long *values_out, int *count_out, int line_number, char *file_name);

int copy_num_data(char **data_img, int *dc, long *values, int count, int bytes_per_val ,int line_number, char *file_name);
int extract_numbers_data(char *str, long *values_out, int *count_out,int line_number,char *file_name);

int make_data_space(char **data_img, int dc, int bytes_to_addint,int line_number,char *file_name);


int extract_copy_asciz_data (char *line_str, char **data_img, int *dc,int line_number,char *file_name);
Instruction *extract_instruction(char **line_ptr);
long build_r_word(int opcode, int rs, int rt, int rd, int funct);
long build_i_word(int opcode, int rs, int rt, int immed);
long build_j_word(int opcode, int reg, int address);
int register_num (char *str,int line_number,char *file_name);
int immediate_to_num(char *str,int line_number,char *file_name);
int process_r_instruction(const Instruction *instr, char *line_ptr, long *coded_word,int line_number,char *file_name);
int process_i_instruction(const Instruction *instr, char *line_ptr, long *coded_word,int line_number,char *file_name);
int process_j_instruction(const Instruction *instr, char *line_ptr, long *coded_word,int line_number,char *file_name);
int add_to_code_image(unsigned char *code_img, int *ic, long coded_word,int line_number, char *file_name);
int process_instruction(char *word_instr, char *line_ptr,  unsigned char *code_img, int *ic,int line_number,char *file_name);

#endif