#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MEMORY_SIZE 4096/* Maximum array size in computer memory */
#define MAX_DATA_IMAGE_SIZE 33550236 /* Maximum code image size (machine memory size minus maximum IC size minus first 100 bytes) */
#define MAX_LINE_LENGTH 82 /* Maximum line length, 80 characters + \n + \0 */
#define MAX_LABEL_LENGTH 32 /* Maximum label length, 31 characters + \0 */
#define MAX_M_L_CHAR 31 /* Maximum  31 characters in macro and label*/
#define MAX_MACRO_LENGTH 32/* Maximum length for name, 31 characters + '\0' */
#define MAX_WORD_LENGTH 32/* Maximum length for any word */
#define MEMORY_ERROR -1/*Returning an error in case of lack of memory space*/
#define IC_START_VALUE 100/* The starting address of the instruction image */
#define DC_START_VALUE 0/* The starting address of the data image */
#define START_VALUE 0/*Initialize to value 0*/
#define END_OF_STRING_CHAR 1/* Size of space for end of string character \0 */
#define TO_LAST_CHAR 1/* Subtract 1 to get to the last character */
#define WRONG_IMMED -32769 /* Value returned when an immediate value is out of range */
#define NOT_REG_VAL -1 /* Value returned when a string is not a valid register */
#define NO_CHAR 0/* no character in string*/
#define REGULAR_BASE 10 /* Regular base for decimal numbers */
#define ON 1 /*Flag on*/
#define OFF 0 /*Flag off*/
#define SUCCESS_F 1 /* Indicates success of a function*/
#define ERROR_F 0 /*Indicates a function failure.*/
#define ERROR_MAIN 1 /*An error was detected in main*/
#define SUCCESS_MAIN 0 /*No errors and success returned for main*/
#define SAME 0 /*result of compare function*/
#define NO_FILE_ARG 1 /* Number of arguments that are not input files */
#define MIN_ARG 2 /* Minimum number of arguments*/
#define FIRST_INDEX 0 /*First index of the array*/
#define ARRAY_UPDATE 0 /*Updating the array to 0 for the next file*/
#define MIN_FILE_NAME_LENGTH 4 /*Minimum valid length for an input file name*/
#define LEN_AS 3 /*The length of the file extension string (.as)*/
#define ERROR_EXIT 1 /*Exiting the program in case of a memory error*/
#define SECOND_BYTE 1 /*The second byte in a 4-byte instruction word */
#define THIRD_BYTE 2 /*The third byte in a 4-byte instruction word */
#define FOURTH_BYTE 3 /*The fourth byte in a 4-byte instruction word */
#define IS_REGISTER 1 /* The operand is a register */
#define IS_NOT_REGISTER 0/* The operand is not a register */
#define IS_MACRO 1 /* The operand is a macro */
#define IS_NOT_MACRO 0 /* The operand is not a macro */
#define IS_NOT_ENTRY 0 /* The operand is not entry*/
#define IS_ENTRY 1 /* The operand is entry*/
#define EXTERN_ADDRESS 0 /*Address value for extern label */

#define BYTES_PER_BYTE 1 /* Number of bytes in one byte */
#define BYTES_PER_WORD 4 /* Number of bytes in a full word*/
#define BYTES_PER_HALF_WORD 2 /* Number of bytes in a half word*/
#define BITS_PER_BYTE 8 /* Number of bits in one byte */
#define MIN_BYTE_VAL -128 /* Minimum value for a signed byte */
#define MAX_BYTE_VAL 127 /* Maximum value for a signed byte */
#define MIN_HALF_WORD_VAL -32768 /* Minimum value for a signed half-word */
#define MAX_HALF_WORD_VAL 32767 /* Maximum value for a signed half-word */
#define MIN_WORD_VAL (-2147483647-1) /* Minimum value for a signed word */
#define MAX_WORD_VAL 2147483647 /* Maximum value for a signed word */
#define MIN_ASCIZ_VAL 0 /* Minimum value for an ASCII character */
#define MAX_ASCIZ_VAL 127 /* Maximum value for an ASCII character */

#define DEF_FNC 0 /* default funct value */
#define ADD_FNC 1 /* funct of add command */
#define SUB_FNC 2 /* funct of sub command */
#define AND_FNC 3 /* funct of and command */
#define OR_FNC 4 /* funct of or command */
#define NOR_FNC 5 /* funct of nor command */
#define MOVE_FNC 1 /* funct of move command */
#define MVHI_FNC 2 /* funct of mvhi command */
#define MVLO_FNC 3 /* funct of mvlo command */

#define R_ALU_OP 0 /* opcode for R-type ALU commands */
#define R_MOVE_OP 1 /* opcode for R-type move commands */
#define ADDI_OP 10 /* opcode for addi command */
#define SUBI_OP 11 /* opcode for subi command */
#define ANDI_OP 12 /* opcode for andi command */
#define ORI_OP 13 /* opcode for ori command */
#define NORI_OP 14 /* opcode for nori command */
#define BNE_OP 15 /* opcode for bne command */
#define BEQ_OP 16 /* opcode for beq command */
#define BLT_OP 17 /* opcode for blt command */
#define BGT_OP 18 /* opcode for bgt command */
#define LB_OP 19 /* opcode for lb command */
#define SB_OP 20 /* opcode for sb command */
#define LW_OP 21 /* opcode for lw command */
#define SW_OP 22 /* opcode for sw command */
#define LH_OP 23 /* opcode for lh command */
#define SH_OP 24 /* opcode for sh command */
#define JMP_OP 25 /* opcode for jmp command */
#define LA_OP 26 /* opcode for la command */
#define CALL_OP 27 /* opcode for call command */
#define HLT_OP 28 /* opcode for hlt command */

#define MASK_OF_BYTE 0xFF /* 8-bit mask (single byte) */
#define MASK_for_SINGLE_BIT 0x01 /* 1-bit mask */
#define ONE_BYTE 8 /*Shift by one byte-8 bits */
#define TWO_BYTES 16 /*Shift by two bytes-16 bits */
#define THREE_BYTES 24 /*Shift by three bytes-24 bits */
#define OP_SHIFT 26 /* Shift for opcode field in instruction word */
#define RS_SHIFT 21 /* Shift for rs field in instruction word */
#define RT_SHIFT 16 /* Shift for rt field in instruction word */
#define RD_SHIFT 11 /* Shift for rd field in instruction word */
#define FUNCT_SHIFT 6 /* Shift for funct field in type r instruction word */
#define REG_SHIFT 25 /* Shift for reg field in type j instruction word */
#define MASK_FOR_IMMED_VAL 0xFFFF /* 16-bit mask (two bytes) */
#define MASK_FOR_ADDRESS_VAL 0x1FFFFFF /* 25-bit mask (three bytes) */
#define NUM_INSTRUCTIONS 27 /* Number of instructions exist in the language */

extern int ICF;/*Final instruction counter value*/
extern int DCF;/*Final data counter value*/


#endif