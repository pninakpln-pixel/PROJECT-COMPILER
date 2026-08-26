
#include "macro.h"
#include "globals.h"


/*
* The function searches for a macro in a linked list by its name.
* Algorithm:
* Run through the list nodes from the beginning to the end.
* Compares the macro name at each node to the requested name.
* Returns the pointer to the node if a match is found, if not- NULL.
*/
NODE_MACRO *find_macro(NODE_MACRO *head, char *name ){
        NODE_MACRO *current= head;/* Pointer to the current node for run the list */
        /* Scan the linked list until it ends */
        while(current!=NULL){
            /*Check if the macro name is the same as the requested name */
            if(strcmp(current->name_macro,name)==SAME){
                return current;
            }
            current=current->next;
        }
        return NULL;
    }
/*
*The function creates a new macro node and inserts it at the head of the linked list.
*Algorithm:
*Dynamic memory allocation for the new node.
*Copy the macro name and initialize the content pointer to NULL.
*Link the new node to the head of the list and update the head of the list.
*/
NODE_MACRO *add_macro(NODE_MACRO **head, char *temp_name)
{
    NODE_MACRO *new_node;
    /* Allocate memory for a new macro node */
    new_node= (NODE_MACRO*)malloc(sizeof(NODE_MACRO));    
    if(new_node==NULL){
        return NULL;
    }
    strcpy(new_node->name_macro, temp_name);/* Copy the macro name to the node */
    new_node->data = NULL;
    new_node->next=*head;/*Update the list*/
    *head = new_node;
    return new_node;
      }

/*
* The function adds a new data line into the macro content at node.
* Assumptions:data is the string of the line to be added.
* Algorithm:
* Calculate the current length of the macro content and the length of the new line.
* Content allocation to merged size.
*Add the new line to the existing content and update the pointer.
*/
int add_line_data(NODE_MACRO *head,char *data){
        int datd_len;/*New content length*/
        int current_len;/*Length of existing content*/
        char *new_data;/*Updated content*/
        
        
        if(head->data!=NULL){
            current_len=strlen(head->data);/* Calculate the length of the content in the macro */
        }else{
            current_len=START_VALUE;
        }
         datd_len= strlen(data);/* Calculate the length of the new line to be added */
        /* Dynamic memory expansion */
        new_data=(char*)realloc(head-> data,current_len+datd_len+1 );
        if(new_data==NULL){
            return ERROR_F;
        }
        /*If this is the first content in the macro, initialize as an empty string before adding*/
        if(current_len==START_VALUE){
           new_data[FIRST_INDEX]='\0'; 
        }
        strcat(new_data,data);
        head->data=new_data;
        return SUCCESS_F;
}
/*
* The function frees all the dynamic memory of the macro list and resets the pointer to NULL.
* Algorithm:
* Run each node in the list and free the data and node struct.
* Reset the original pointer of the head of the list to NULL when finished.
*/
void free_macro(NODE_MACRO **head){
    NODE_MACRO *current=*head;/* Get the address of the head of the list */
    NODE_MACRO *temp;/*Temp to save node */
    /*Scan the list and free */
    while(current!=NULL){
        temp=current;
        current=current->next;
        /* free the macro content if it exist */
        if (temp->data!= NULL)
        {
            free(temp->data);
        }
        free(temp);

    }
    *head = NULL;
}


    
    
    



