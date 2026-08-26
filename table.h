#ifndef TABLE_H
#define TABLE_H

#include "globals.h"
/* צומת ברשימה מקושרת של סמל */
/* סוגי סמלים בטבלה */
int add_ext_ent(EXT_ENT_NODE **head, char *name, int address);
void free_ext_ent(EXT_ENT_NODE *head);
/*Symbol *find_symbol(Symbol *head, char *name);*/

Symbol *find_symbol(Symbol *head, char *name);
int update_data_symbols_address(Symbol *head, int ic);
int add_symbol(Symbol **head, char *name, int address, SymbolType type,int line_number, char *file_name);
int free_symbol(Symbol **head);
const Instruction *find_instruction(char *cmd_name);

#endif