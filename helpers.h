#ifndef HELPERS_H
#define HELPERS_H

/*
typedef enum {
    TYPE_R_ALU,        
    TYPE_R_MOVE,
    TYPE_I_ALU,        
    TYPE_I_BRANCH,     
    TYPE_I_LOAD_STORE, 
    TYPE_J             
} InstructionType;*/
/*
typedef struct Instruction {
    char *name;         
    InstructionType type;
    int opcode;          
    int funct;          
} Instruction;*/


int is_empty_line( char *str);
char *extract_word(char *str, char *result,int is_operand);
int is_label(char *word);
int is_comment(char *str);
char *make_new_name_file(char *name_file, char *ending);
int is_valide_name(char *name,int line_number,char *file_name, int is_macro);
/*Instruction *find_instruction(char *word);*/
#endif