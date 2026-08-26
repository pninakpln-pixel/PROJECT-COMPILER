
#include "table.h"
#include "globals.h"
#include "helpers.h"


int add_ext_ent(EXT_ENT_NODE **head, char *name, int address){
    EXT_ENT_NODE *new_node=(EXT_ENT_NODE *)malloc(sizeof(EXT_ENT_NODE));
    if(new_node == NULL) 
    { 
        return MEMORY_ERROR;
    }
    strcpy(new_node->name,name);
    new_node->address =address;
    new_node->next=*head;
    *head=new_node;
    return SUCCESS_F;
}



void free_ext_ent(EXT_ENT_NODE *head) {
    EXT_ENT_NODE *current = head;
    EXT_ENT_NODE *temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
}

/*Function to find a symbol in the list by this name*/
Symbol *find_symbol(Symbol *head, char *name) {
    Symbol *current = head;

    while (current != NULL) {
        if (strcmp(current->name, name)==SAME)
            return current;
        current = current->next;
    }

    return NULL;
}


/*Function to insert a new symbol at the beginning of the list*/
int add_symbol(Symbol **head, char *name, int address, SymbolType type, int line_number, char *file_name) {
    Symbol *new_symbol;

    if (!is_valide_name(name, line_number, file_name,IS_NOT_MACRO)) return ERROR_F;

    if (find_symbol(*head, name) != NULL) {
        if (type != SYMBOL_EXTERN)
            fprintf(stderr,"Error at file: %s, line %d:\nSymbol '%s' is already defined.\n", file_name, line_number, name);
        else if ((find_symbol(*head, name))->type != SYMBOL_EXTERN)
            fprintf(stderr,"Error at file: %s, line %d:\nSymbol '%s' is already defined as not external.\n" ,file_name, line_number, name);
        return ERROR_F;
    }

    new_symbol = (Symbol *)malloc(sizeof(Symbol));
    if (new_symbol == NULL) {
        fprintf(stderr,"Error at file: %s, line %d:\nMemory allocation failed for symbol %s\n", file_name, line_number, name);
        return MEMORY_ERROR;
    }

    strcpy(new_symbol->name, name);
    new_symbol->address = address;
    new_symbol->type = type;
    new_symbol->is_entry = IS_NOT_ENTRY;

    new_symbol->next = *head;
    *head = new_symbol;

    return SUCCESS_F; 
}

/*After first pass add ic to the address of every data symbol*/
int update_data_symbols_address(Symbol *head,int ic) {
    Symbol *current = head;

    while (current != NULL) {
        if (current->type == SYMBOL_DATA)
            (current->address) +=ic;
        current = current->next;
    }
    return SUCCESS_F;
}


int free_symbol(Symbol **head) {
    Symbol *current = *head;
    Symbol *next_node;

    while (current != NULL) {
        next_node = current->next;

        free(current);
        current = next_node;
    }

    *head = NULL;
    return SUCCESS_F;
}

const Instruction INSTRUCTION_TABLE[] = {
    /* פקודות R */
    {"add",  TYPE_R_ALU, R_ALU_OP, ADD_FNC},
    {"sub",  TYPE_R_ALU, R_ALU_OP, SUB_FNC},
    {"and",  TYPE_R_ALU, R_ALU_OP, AND_FNC},
    {"or",   TYPE_R_ALU, R_ALU_OP, OR_FNC},
    {"nor",  TYPE_R_ALU, R_ALU_OP, NOR_FNC},
    {"move", TYPE_R_MOVE, R_MOVE_OP, MOVE_FNC},
    {"mvhi", TYPE_R_MOVE, R_MOVE_OP, MVHI_FNC},
    {"mvlo", TYPE_R_MOVE, R_MOVE_OP, MVLO_FNC},

    /* פקודות I */
    {"addi", TYPE_I_ALU, ADDI_OP, DEF_FNC},
    {"subi", TYPE_I_ALU, SUBI_OP, DEF_FNC},
    {"andi", TYPE_I_ALU, ANDI_OP, DEF_FNC},
    {"ori",  TYPE_I_ALU, ORI_OP, DEF_FNC},
    {"nori", TYPE_I_ALU, NORI_OP, DEF_FNC},
    {"bne",  TYPE_I_BRANCH, BNE_OP, DEF_FNC},
    {"beq",  TYPE_I_BRANCH, BEQ_OP, DEF_FNC},
    {"blt",  TYPE_I_BRANCH, BLT_OP, DEF_FNC},
    {"bgt",  TYPE_I_BRANCH, BGT_OP, DEF_FNC},
    {"lb",   TYPE_I_LOAD_STORE, LB_OP, DEF_FNC},
    {"sb",   TYPE_I_LOAD_STORE, SB_OP, DEF_FNC},
    {"lw",   TYPE_I_LOAD_STORE, LW_OP, DEF_FNC},
    {"sw",   TYPE_I_LOAD_STORE, SW_OP, DEF_FNC},
    {"lh",   TYPE_I_LOAD_STORE, LH_OP, DEF_FNC},
    {"sh",   TYPE_I_LOAD_STORE, SH_OP, DEF_FNC},

    /* פקודות J */
    {"jmp",  TYPE_J_JUMP, JMP_OP, DEF_FNC},
    {"la",   TYPE_J_LOAD_ADD, LA_OP, DEF_FNC},
    {"call", TYPE_J_CALL, CALL_OP, DEF_FNC},
    {"hlt", TYPE_J_HLT, HLT_OP, DEF_FNC}
};


const Instruction *find_instruction(char *cmd_name) {
    int i;

    if (cmd_name == NULL) {
        return NULL;
    }

    /* סריקת טבלת ההוראות והשוואת השם */
    for (i = 0; i < NUM_INSTRUCTIONS; i++) {
        if (strcmp(INSTRUCTION_TABLE[i].name, cmd_name)==SAME) {
            return &INSTRUCTION_TABLE[i]; /* נמצאה התאמה */
        }
    }

    return NULL; /* הפקודה לא נמצאה */
}

