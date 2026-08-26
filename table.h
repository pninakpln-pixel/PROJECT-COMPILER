#ifndef TABLE_H
#define TABLE_H

#include "globals.h"

/*
* This file defines the structures and functions for managing assembler tables.
* Includes the symbol table, entry and extern lists,
* and the machine instruction table .
*/

/*Structure for a node in a linked list to store extern or entry for output files */
typedef struct EXT_ENT_NODE{
    char name[MAX_LABEL_LENGTH];/* Label name */
    int address;/* Memory address of the label */
    struct EXT_ENT_NODE *next;/* Pointer to the next node in the list */
}EXT_ENT_NODE;

/* Possible symbol types in the symbol table */
typedef enum {
    SYMBOL_CODE,  /* Symbol for code instruction */
    SYMBOL_DATA,  /* Data instruction symbol */
    SYMBOL_EXTERN /* External label symbol */
} SymbolType;


/* Structure of a node in the symbol table */
typedef struct Symbol {
    char name[MAX_LABEL_LENGTH];/*Symbol name*/    
    int address;/*The symbol address in memory */         
    SymbolType type;/*Symbol type (CODE, DATA, EXTERN) */     
    int is_entry;/*Flag if the symbol was defined as entry*/       
    struct Symbol *next;/*Pointer to the next symbol in the list */  
} Symbol;

/*Division into types of assembler instructions*/
typedef enum {
    TYPE_R_ALU, /*R-type arithmetic logical commands */
    TYPE_R_MOVE,/*R-type copy commands */
    TYPE_I_ALU,/*Arithmetic instructions with immediate value type I */
    TYPE_I_BRANCH,/*Type I branch commands */
    TYPE_I_LOAD_STORE, /*Type I memory load and store commands*/
    TYPE_J_JUMP,/*J-type jump commands */
    TYPE_J_LOAD_ADD,/*Load address la command of type J */
    TYPE_J_CALL,/*Call command for a J type */
    TYPE_J_HLT/* hlt type J run stop command */
} InstructionType;

/* Structure of language instructions */
typedef struct Instruction{
    char *name;/* Assembly command name*/     
    InstructionType type;/*The structure type of the command*/
    int opcode;         
    int funct;          
} Instruction;

/* צומת ברשימה מקושרת של סמל */
/* סוגי סמלים בטבלה */
int add_ext_ent(EXT_ENT_NODE **head, char *name, int address);
void free_ext_ent(EXT_ENT_NODE *head);


Symbol *find_symbol(Symbol *head, char *name);
int update_data_symbols_address(Symbol *head, int ic);
int add_symbol(Symbol **head, char *name, int address, SymbolType type,int line_number, char *file_name);
int free_symbol(Symbol **head);
const Instruction *find_instruction(char *cmd_name);

#endif