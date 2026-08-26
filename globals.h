#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MEMORY_SIZE 4096/* Maximum array size in computer memory */
#define MAX_LINE_LENGTH 82 /* Maximum line length, 80 characters + \n + \0 */
#define MAX_LABEL_LENGTH 32 /* Maximum label length, 31 characters + \0 */
#define MAX_MACRO_LENGTH 32/* Maximum length for name, 31 characters + '\0' */
#define MAX_WORD_LENGTH 32/* Maximum length for any word */
#define MEMORY_ERROR -1/*Returning an error in case of lack of memory space*/
#define IC_START_VALUE 100/* The starting address of the instruction image */
#define DC_START_VALUE 0/* The starting address of the data image */
#define START_VALUE 0/*Initialize to value 0*/

#define MAX_DATA_IMAGE_SIZE 33,550,236 /* Maximum code image size (machine memory size minus maximum IC size minus first 100 bytes) */



#define MIN_BYTE_VAL 0x80 /* Minimum value for a signed byte */
#define MAX_BYTE_VAL 0x7F /* Maximum value for a signed byte */
#define MIN_HALF_WORD_VAL 0x8000 /* Minimum value for a signed half-word */
#define MAX_HALF_WORD_VAL 0x7FFF /* Maximum value for a signed half-word */
#define MIN_WORD_VAL 0x80000000 /* Minimum value for a signed word */
#define MAX_WORD_VAL 0x7FFFFFFF /* Maximum value for a signed word */

#define REGULAR_BASE 10 /* Regular base for decimal numbers */

#define ON 1/*Flag on*/
#define OFF 0 /*Flag off*/
#define TRUE 1 /*Returns a logical true value.*/
#define FALSE 0/*Returns a logical false value.*/
#define SUCCESS_F 1/* Indicates success of a function*/
#define ERROR_F 0/*Indicates a function failure.*/
#define ERROR_MAIN 1/*An error was detected in main*/
#define SUCCESS_MAIN 0/*No errors and success returned for main*/

#define NO_FILE_ARG 1/* Number of arguments that are not input files */
#define MIN_ARG 2 /* Minimum number of arguments*/
#define FIRST_INDEX 0/*First index of the array*/
#define ARRAY_UPDATE 0 /*Updating the array to 0 for the next file*/
#define MIN_FILE_NAME_LENGTH 4/*Minimum valid length for an input file name*/
#define LEN_AS 3/*The length of the file extension string (.as)*/
#define ERROR_EXIT 1/*Exiting the program in case of a memory error*/
#define SECOND_BYTE 1/*The second byte in a 4-byte instruction word */
#define THIRD_BYTE 2/*The third byte in a 4-byte instruction word */
#define FOURTH_BYTE 3/*The fourth byte in a 4-byte instruction word */

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


extern int ICF;/*Final instruction counter value*/
extern int DCF;/*Final data counter value*/


typedef struct EXT_ENT_NODE{
    char name[MAX_NAME_LENGTH];
    int address;
    struct EXT_ENT_NODE *next;
}EXT_ENT_NODE;


typedef enum {
    SYMBOL_CODE,  
    SYMBOL_DATA,  
    SYMBOL_EXTERN 
} SymbolType;



typedef struct Symbol {
    char name[32];        
    int address;         
    SymbolType type;     
    int is_entry;        
    struct Symbol *next;  
} Symbol;

typedef enum {
    TYPE_R_ALU,   /* פקודות אריתמטיות/לוגיות R */
    TYPE_R_MOVE,
    TYPE_I_ALU,   /* פקודות אריתמטיות עם מיידי */
    TYPE_I_BRANCH,/* פקודות התניה כמו beq, bne */
    TYPE_I_LOAD_STORE, /* load/store */
    TYPE_J_JUMP,  /* פקודות קפיצה J */
    TYPE_J_LOAD_ADD,
    TYPE_J_CALL,
    TYPE_J_HLT
} InstructionType;

typedef struct Instruction{
    char *name;         
    InstructionType type;
    int opcode;         
    int funct;          
} Instruction;


#define NUM_INSTRUCTIONS 27

extern int ICF;
extern int DCF;

#endif