#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "table.h"
int second_pass(char *file_name,Symbol *symbol_list,unsigned char *code_img,EXT_ENT_NODE **ext_list, EXT_ENT_NODE **ent_list);
#endif
