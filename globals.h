#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MEMORY_SIZE 4096/* Maximum array size in computer memory */
#define MAX_LINE_LENGTH 82 /* Maximum line length: 80 characters + \n + \0 */
#define MAX_LABEL_LENGTH 32 /* Maximum label length: 31 characters + \0 */
#define MAX_NAME_LENGTH 32/* Maximum length for name, 31 characters + '\0' */

#define IC_START_VALUE 100/* The starting address of the instruction image */
#define DC_START_VALUE 0/* The starting address of the data image */


#define TRUE 1 /*Returns a logical true value.*/
#define FALSE 0/*Returns a logical false value.*/
#define SUCCESS_F 1/* Indicates success of a function*/
#define ERROR_F 0/*Indicates a function failure.*/

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