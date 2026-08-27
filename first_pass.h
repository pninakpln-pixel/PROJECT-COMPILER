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

/*
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

/*
 * Copies extracted numeric values into the data image array and updates the data counter.
 * Inputs: 
 * - data_img - double pointer to the data image buffer.
 * - dc - pointer to the data counter.
 * - values - array of the numeric values to copy.
 * - count - number of values in the array.
 * - bytes_per_val - size of each value in bytes (1, 2, or 4).
 * - line_number - current line number in source file.
 * - file_name - name of the processed file.
 * Output: SUCCESS_F (1) on success, MEMORY_ERROR (-1) for memory reallocation failure, and ERROR_F (0) else.
 */
int copy_num_data(char **data_img, int *dc, long *values, int count, int bytes_per_val ,int line_number, char *file_name);

/*
 * Extracts a string enclosed in double quotes from an .asciz directive and adds it to the data image.
 * Inputs: 
 * - line_str - pointer to the directive arguments in the source line.
 * - data_img - double pointer to the data image buffer.
 * - dc - pointer to the data counter.
 * - line_number - current line number in source file.
 * - file_name - name of the processed file.
 * Output: SUCCESS_F (1) on success, MEMORY_ERROR (-1) for memory reallocation failure, ERROR_F (0) else.
 */
int extract_copy_asciz_data (char *line_str, char **data_img, int *dc,int line_number,char *file_name);

/*
 * Reallocates and expand the dynamic memory space for the data image.
 * Inputs: 
 * - data_img - double pointer to the data image memory buffer.
 * - dc - current data counter value.
 * - bytes_to_add - number of new bytes to allocate.
 * - line_number - current line number in the source file.
 * - file_name - name of the processed file.
 * Output: SUCCESS_F (1) on successful allocation, MEMORY_ERROR (-1) for memory reallocation failure.
 */
int make_data_space(char **data_img, int dc, int bytes_to_add,int line_number,char *file_name);

/*
 * Send instruction procesing based on category (R, I, J) and add the generated machine word to code image.
 * Inputs: 
 * - word_instr - name of the instruction.
 * - line_ptr - pointer to remaining operand string in the line.
 * - code_img - pointer to code image memory buffer.
 * - ic - pointer to instruction counter.
 * - line_number - current line number in source file.
 * - file_name - name of the processed file.
 * Output: SUCCESS_F (1) on successful processing, MEMORY_ERROR (-1) for memory failure, ERROR_F else.
 */
int process_instruction(char *word_instr, char *line_ptr,  unsigned char *code_img, int *ic,int line_number,char *file_name);

/*
 * Writes a 32-bit encoded instruction word into the code image memory array and increments the instruction counter.
 * Inputs: 
 * - code_img - pointer to the code image memory array.
 * - ic - pointer to the instruction counter.
 * - coded_word - 32-bit encoded machine code word to add.
 * - line_number - current line number in source file.
 * - file_name - name of the processed file.
 * Output: SUCCESS_F (1) on successful insertion, MEMORY_ERROR (-1) on memory overflow limits.
 */
int add_to_code_image(unsigned char *code_img, int *ic, long coded_word,int line_number, char *file_name);

/*
 * Proceses operands of an R-type instruction, validates register arguments, and constructs the encoded word.
 * Inputs: 
 * - instr - pointer to instruction structure.
 * - line_ptr - pointer to operand text in source line.
 * - coded_word - pointer to store the resulting 32-bit encoded instruction word.
 * - line_number - current line number in source file.
 * - file_name - name of the processed file.
 * Output: SUCCESS_F (1) on valid syntax, ERROR_F (0) if there errors.
 */
int process_r_instruction(const Instruction *instr, char *line_ptr, long *coded_word,int line_number,char *file_name);

/*
 * Proceses operands of an I-type instruction, validates registers and immediate values, and constructs the encoded word.
 * Inputs: 
 * - instr - pointer to instruction structure.
 * - line_ptr - pointer to operand text in source line.
 * - coded_word - pointer to store the resulting 32-bit encoded instruction word.
 * - line_number - current line number in source file.
 * - file_name - name of the processed file.
 * Output: SUCCESS_F (1) on valid syntax, ERROR_F (0) if there errors.
 */
int process_i_instruction(const Instruction *instr, char *line_ptr, long *coded_word,int line_number,char *file_name);

/*
 * Proceses operands of a J-type instruction, validates register or target label, and constructs the encoded word.
 * Inputs: 
 * - instr - pointer to instruction structure.
 * - line_ptr - pointer to operand text in source line.
 * - coded_word - pointer to store the resulting 32-bit encoded instruction word.
 * - line_number - current line number in source file.
 * - file_name - name of the processed file.
 * Output: SUCCESS_F (1) on valid syntax, ERROR_F (0) if there errors.
 */
int process_j_instruction(const Instruction *instr, char *line_ptr, long *coded_word,int line_number,char *file_name);

/*
 * Proceses a register token string and converts it to its integer index.
 * Inputs: 
 * - str - string representing register operand (like "$5").
 * - line_number - current line number in source file.
 * - file_name - name of the processed file.
 * Output: Register index (0-31) on success, NOT_REG_VAL (-1) on invalid register syntax or out-of-bounds index.
 */
int register_num (char *str,int line_number,char *file_name);

/*
 * Converts an immediate value string operand into an integer format.
 * Inputs: 
 * - str - string representing immediate numeric value.
 * - line_number - current line number in source file.
 * - file_name - name of the processed file.
 * Output: Integer value of immediate on success, WRONG_IMMED on invalid numeric input.
 */
int immediate_to_num(char *str,int line_number,char *file_name);

/*
 * Encodes R-type instruction fields into a single 32-bit binary instruction word.
 * Inputs: 
 * - opcode - operation code.
 * - rs - source register number.
 * - rt - target register number.
 * - rd - destination register number.
 * - funct - function code field.
 * Output: 32-bit encoded machine code word as a long.
 */
long build_r_word(int opcode, int rs, int rt, int rd, int funct);

/*
 * Encodes I-type  instruction fields into a single 32-bit binary instruction word.
 * Inputs: opcode - operation code.
 *         rs - source register number.
 *         rt - target register number.
 *         immed - 16-bit immediate value.
 * Output: 32-bit encoded machine code word as a long.
 */
long build_i_word(int opcode, int rs, int rt, int immed);

/*
 * Encodes J-type MIPS instruction fields into a single 32-bit binary instruction word.
 * Inputs: opcode - operation code.
 *         reg - flag indicate register operand mode.
 *         address - 26-bit target address or register number.
 * Output: 32-bit encoded machine code word as a long.
 */
long build_j_word(int opcode, int reg, int address);

Instruction *extract_instruction(char **line_ptr);

#endif