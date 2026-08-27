#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "globals.h"
#include "table.h"

/*
* The function makes the second pass in the assembler to complete the  code image and collect extern and entry symbols.
* Input:
* file_name: The name of the .am file.
* symbol_list:Pointer to the symbol table.
* code_img: The byte array of the machine code image.
* ext_list:Double pointer to the list of extern.
* ent_list:Double pointer to the list of entry.
* Output:
* Returns SUCCESS_F if successful.
* Returns ERROR_F if errors were found in the file .
* Returns MEMORY_ERROR in case of a failure in dynamic memory allocation.
*/

int second_pass(char *file_name,Symbol *symbol_list,unsigned char *code_img,EXT_ENT_NODE **ext_list, EXT_ENT_NODE **ent_list);
#endif
