#include "globals.h"
#include "helpers.h"


int is_empty_line( char *str) {
    while (*str!= '\0') {
        if (isspace((unsigned char)*str)==0) {
            return 0; 
        }
        str++;
    }
    return 1;
}

char *extract_word(char *str, char *result, int is_operand){
    int i = 0;
    int j = 0;

    while (str[i]!='\0' && isspace((unsigned char)str[i])!=0){
        i++;
    }
    
    while (str[i] != '\0' && isspace((unsigned char)str[i])==0){
        if(is_operand==1 && str[i]==',') {
            break;
        }
        result[j] = str[i];
        i++;
        j++;
    }
    result[j] = '\0';
    
    return &str[i];
}

int is_label(char *word) {
    int length = strlen(word);
    if(length > 0 && word[length- 1] == ':'){
        return 1;
    }
 return 0;
}

int is_comment(char *str) {
    while (*str!= '\0' && isspace((unsigned char)*str)!= 0) {
        str++;
    }
    if (*str == ';') {
        return 1; 
    } else {
        return 0; 
    }
}

char *make_new_name_file(char *name_file, char *ending){
    int len;
    char *new_name_file;
    new_name_file=(char *)malloc(strlen(name_file)+strlen(ending)+1);
    if(new_name_file==NULL)
        return NULL;

    strcpy(new_name_file,name_file);
    len=(int)strlen(new_name_file);

    if (len >= 3 && strcmp(new_name_file+ len - 3,".as") == 0) {
        new_name_file[len-3] ='\0';
    } 
    strcat(new_name_file,ending);
    
    return new_name_file;
}

int is_valide_name(char *name,int line_number,char *file_name, int is_mac){
    int i;
    static const char *reserved_words[]={
        
        "add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo",
        
        "addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt",
        "lb", "sb", "lw", "sw", "lh", "sh",
        
        "jmp", "la", "call", "hlt",
        
        ".db", ".dw", ".dh", ".asciz", ".entry", ".extern",
        
        "mcro", "mcroend",
        NULL
    };

    if(name == NULL || strlen(name) == 0) {
        fprintf(stderr,"There is an error in file %s at line %d, name is missing.\n",file_name, line_number );
        return 0;}

    if(strlen(name) > 31) {
        fprintf(stderr,"There is an error in file %s at line %d,the name '%s' more then 31 characters.\n",file_name,line_number,name);
        return 0;
    }

    if(!isalpha(name[0])){
        fprintf(stderr,"There is an error in file %s at line %d, the name '%s' must start with an alphabetic letter.\n",file_name,line_number,name);
        return 0;
    }


    for(i=1; name[i]!='\0'; i++) {
        if(is_mac==1){
            if(!isalnum(name[i]) && name[i] != '_') {
                fprintf(stderr,"There is an error in file %s at line %d, the name '%s' with invalid characters.\n",file_name,line_number,name);
                return 0;
            }
        }else{
            if(!isalnum(name[i])) {
                fprintf(stderr,"There is an error in file %s at line %d, the name '%s' with invalid characters.\n",file_name,line_number,name);
                return 0;
            }

        }
    }
    for(i = 0; reserved_words[i] != NULL; i++) {
        if(strcmp(name, reserved_words[i]) == 0) {
            fprintf(stderr,"There is an error in file %s at line %d, the name '%s' is a reserved word.\n",file_name,line_number,name);
            return 0;
        }
    }

 return 1;

}
