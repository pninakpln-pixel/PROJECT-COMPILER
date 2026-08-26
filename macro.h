#ifndef MACRO_H
#define MACRO_H

#include "globals.h"

typedef struct NODE_MACRO{
    char name_macro[32];
    char *data;
    struct NODE_MACRO *next;
}NODE_MACRO;

NODE_MACRO *find_macro(NODE_MACRO *head, char *name );
NODE_MACRO *add_macro(NODE_MACRO **head, char *temp_name);
int add_line_data(NODE_MACRO *head, char *data);
void free_macro(NODE_MACRO *head);

#endif