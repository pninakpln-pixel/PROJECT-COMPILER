typedef struct Node_macro{
    char name_macro[81];
    char *data;
    struct Node_macro *next;

}Node_macro;

Node_macro *add_macro(Node_macro *head, char *temp_name){

    Node_macro *new_node= (Node_macro)malloc(sizeof(Node_macro));

    if(new_node==Null)
      return 1;
    
      if(find_macro(head, name_macro)!=NULL)(
        printf("error! two macros with the same name")
        return NULL;)
    strncpy(new_node->, name, 80)
    new_node->name[80] = '\0';
    new_node->data = NULL;

    new_node->next=head;
    return new_node;
      }

    Node_macro *find_macro(Node_macro *head, char *name ){
        Node_macro *current= head;
        while(current){
            if(strcmp(current->name_macro,name)==0)
                return current;
            current=current->next;

        }
        return NULL;
    }

    int add_line_data(Node_macro *head, char *data){
        int current_len= head->data ? strlen(head->data) : 0;
        int datd_len= strlen(data);
        
        char *new_data=(char *)realloc(head-> data,current_len+datd_len+1 );
        if(new_data==NULL){
            printf("error, memory allocation didnt success")
            return 1;}
        if(current_len==0){
           new_data[0] = '\0'; 
        }
        strcat(new_data,data);
        node->data=new_data;
    }

    
    

}

