
#include "table.h"
#include "globals.h"
#include "helpers.h"


int add_ext_ent(EXT_ENT_NODE **head, char *name, int address){
    EXT_ENT_NODE *new_node=(EXT_ENT_NODE *)malloc(sizeof(EXT_ENT_NODE));
    if(new_node == NULL) 
    { 
        return 0;
    }
    strncpy(new_node->name, name,31);
    new_node->name[31] ='\0';
    new_node->address = address;
    new_node->next = *head;
    *head = new_node;
    return 1;
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
        if (strcmp(current->name, name) == 0)
            return current;
        current = current->next;
    }

    return NULL;
}


/*Function to insert a new symbol at the beginning of the list*/
int add_symbol(Symbol **head, char *name, int address, SymbolType type, int line_number, char *file_name) {
    Symbol *new_symbol;

    if (!is_valide_name(name, line_number, file_name, 0)) return 0;

    if (find_symbol(*head, name) != NULL) {
        if (type != SYMBOL_EXTERN)
            fprintf(stderr,"Error at file: %s, line %d:\nSymbol '%s' is already defined.\n", file_name, line_number, name);
        else if ((find_symbol(*head, name))->type != SYMBOL_EXTERN)
            fprintf(stderr,"Error at file: %s, line %d:\nSymbol '%s' is already defined as not external.\n" ,file_name, line_number, name);
        return 0;
    }

    new_symbol = (Symbol *)malloc(sizeof(Symbol));
    if (new_symbol == NULL) {
        fprintf(stderr,"Error at file: %s, line %d:\nMemory allocation failed for symbol %s\n", file_name, line_number, name);
        return -1;
    }

    strcpy(new_symbol->name, name);
    new_symbol->address = address;
    new_symbol->type = type;
    new_symbol->is_entry = 0;

    new_symbol->next = *head;
    *head = new_symbol;

    return 1; 
}

/*After first pass add ic to the address of every data symbol*/
int update_data_symbols_address(Symbol *head, int ic) {
    Symbol *current = head;

    while (current != NULL) {
        if (current->type == SYMBOL_DATA)
            (current->address) += ic;
        current = current->next;
    }
    return 1;
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
    return 0;
}
const Instruction INSTRUCTION_TABLE[] = {
    /* פקודות R */
    {"add",  TYPE_R_ALU, 0, 1},
    {"sub",  TYPE_R_ALU, 0, 2},
    {"and",  TYPE_R_ALU, 0, 3},
    {"or",   TYPE_R_ALU, 0, 4},
    {"nor",  TYPE_R_ALU, 0, 5},
    {"move", TYPE_R_MOVE, 1, 1},
    {"mvhi", TYPE_R_MOVE, 1, 2},
    {"mvlo", TYPE_R_MOVE, 1, 3},

    /* פקודות I */
    {"addi", TYPE_I_ALU, 10, 0},
    {"subi", TYPE_I_ALU, 11, 0},
    {"andi", TYPE_I_ALU, 12, 0},
    {"ori",  TYPE_I_ALU, 13, 0},
    {"nori", TYPE_I_ALU, 14, 0},
    {"bne",  TYPE_I_BRANCH, 15, 0},
    {"beq",  TYPE_I_BRANCH, 16, 0},
    {"blt",  TYPE_I_BRANCH, 17, 0},
    {"bgt",  TYPE_I_BRANCH, 18, 0},
    {"lb",   TYPE_I_LOAD_STORE, 19, 0},
    {"sb",   TYPE_I_LOAD_STORE, 20, 0},
    {"lw",   TYPE_I_LOAD_STORE, 21, 0},
    {"sw",   TYPE_I_LOAD_STORE, 22, 0},
    {"lh",   TYPE_I_LOAD_STORE, 23, 0},
    {"sh",   TYPE_I_LOAD_STORE, 24, 0},

    /* פקודות J */
    {"jmp",  TYPE_J_JUMP, 30, 0},
    {"la",   TYPE_J_LOAD_ADD, 31, 0},
    {"call", TYPE_J_CALL, 32, 0},
    {"hlt", TYPE_J_HLT, 63, 0}
};


const Instruction *find_instruction(char *cmd_name) {
    int i;

    if (cmd_name == NULL) {
        return NULL;
    }

    /* סריקת טבלת ההוראות והשוואת השם */
    for (i = 0; i < NUM_INSTRUCTIONS; i++) {
        if (strcmp(INSTRUCTION_TABLE[i].name, cmd_name) == 0) {
            return &INSTRUCTION_TABLE[i]; /* נמצאה התאמה */
        }
    }

    return NULL; /* הפקודה לא נמצאה */
}

