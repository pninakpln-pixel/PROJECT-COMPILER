
#include "macro.h"
#include "globals.h"



NODE_MACRO *find_macro(NODE_MACRO *head, char *name ){
        NODE_MACRO *current= head;
        while(current!=NULL){
            if(strcmp(current->name_macro,name)==0){
                return current;
            }
            current=current->next;
        }
        return NULL;
    }

NODE_MACRO *add_macro(NODE_MACRO **head, char *temp_name)
{
    NODE_MACRO *new_node;
    
    new_node= (NODE_MACRO*)malloc(sizeof(NODE_MACRO));    
    
    if(new_node==NULL){
        
        return NULL;
    }

    strncpy(new_node->name_macro, temp_name, 31);
    new_node->name_macro[31] ='\0';
    new_node->data = NULL;

    new_node->next=*head;
    *head = new_node;
    return new_node;
      }


int add_line_data(NODE_MACRO *head, char *data){
        int datd_len;
        int current_len;
        char *new_data;
        
        
        if(head->data!=NULL){
            current_len=strlen(head->data);
        } else{
            current_len=0;
        }
         datd_len= strlen(data);
        
        new_data=(char *)realloc(head-> data,current_len+datd_len+1 );
        if(new_data==NULL){
            return 0;
        }
        if(current_len==0){
           new_data[0] = '\0'; 
        }
        strcat(new_data,data);
        head->data=new_data;
        return 1;}

void free_macro(NODE_MACRO *head){
    NODE_MACRO *current=head;
    NODE_MACRO *temp;
    while(current!=NULL){
        temp=current;
        current=current->next;
        if (temp->data!= NULL)
        {
            free(temp->data);
        }
        free(temp);

    }
}


    
    
    



