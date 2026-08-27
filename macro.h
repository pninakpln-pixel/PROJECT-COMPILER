#ifndef MACRO_H
#define MACRO_H
#include "globals.h"
/*
*This file defines the struct and functions for managing the macro table in the pre_assembler.
*In the file has functions for searching for a macro in a linked list, adding a new macro, adding the macro's data lines, and free memory.
* Input: pointers to a macro list, macro names, and data lines.
* Output: pointers to macro nodes, and returne success/error.
*/

/*Struct of a node in a linked list of macros.*/
typedef struct NODE_MACRO{
    char name_macro[MAX_MACRO_LENGTH];/*String array to store the macro name*/
    char *data;/* Pointer to the data content of the macro */
    struct NODE_MACRO *next;/*Pointer to the next macro node in the linked list.*/
}NODE_MACRO;

/*
* Searches for a macro in the linked list by name.
* Input:pointer to the top of the macro list, and the string name that the macro search for.
* Output:pointer to the node of the macro found, or NULL if not in the list.
*/
NODE_MACRO *find_macro(NODE_MACRO *head, char *name );

/*
* Allocates and add a new macro node to the top of the linked list.
* Input:double pointer to the top of the list, and the string name of the new macro.
* Output:pointer to the new macro node, or NULL if the memory allocation failed.
*/
NODE_MACRO *add_macro(NODE_MACRO **head, char *temp_name);

/*
* Adds a new content line to the data string of an existing macro.
* Input:pointer to the macro node, the string of the line to add.
* Output: Returns SUCCESS_F if it was successful, or ERROR_F in case of a memory allocation failed.
*/
int add_line_data(NODE_MACRO *head, char *data);

/*
* Frees all dynamic memory allocated of the macro list and resets the head of the list to NULL.
* Input:double pointer to the head of the macro list.
* Output:no.
*/
void free_macro(NODE_MACRO **head);

#endif