#include "globals.h"
#include "helpers.h"

/*
* The function checks if the current line is only spaces or empty.
* The input str is a valid string endin with '\0'.
* The algorithm:
*  We run over the string with pointer.
*  If we see a non-whitespace character, we return ERROR_F-0.
*  If we end the string, we return SUCCESS_F-1.
*/
int is_empty_line(char *str){
    /* Run on each character in the string until the end of the line */
    while(*str!='\0'){
        /* Check if the current character is not a whitespace character */
        if(isspace(*str)==ERROR_F){
            return ERROR_F; 
        }
        str++;/* Move to the next character */
    }
    return SUCCESS_F;
}

/*
* The function extracts the next word in the line.
* The algorithm:
* Skip whitespace until the word.
* Copy character by character to result until a space or comma depending on the value of is_operand.
* Close with '\0' and return a pointer to continue.
*/
char *extract_word(char *str,char *result,int is_operand){
    int i=START_VALUE;/* Index to read from the source string */
    int j=START_VALUE;/* Index to write to the result*/
    /* Skip white spaces at the beginning of the word */
    while(str[i]!='\0' && isspace(str[i])!=ERROR_F){
        i++;
    }
    
    while(str[i]!='\0' && isspace(str[i])==ERROR_F){
        /* If it is a register operand and there is a comma, stop the extraction */
        if(is_operand==IS_REGISTER && str[i]==','){
            break;
        }
        result[j]=str[i];/* Copy the current character */
        i++;
        j++;
    }
    result[j]='\0';/* Close the result string */
    
    return &str[i];/* Return a pointer to the location where we stopped */
}
/*
* The function checks if the word is a label definition ':'
*The algorithm:
* Calculate the length of the word.
* Check that the last character is ':' and return an ERROR_F if it not */
int is_label(char *word){
    int length = strlen(word);/* Check word length */
    /* Check that the word is not empty and its last character is ':' */
    if(length>0 && word[length-TO_LAST_CHAR]==':'){
        return SUCCESS_F;
    }
 return ERROR_F;
}
/*
* The function checks if the line is a comment line - starting with ';'.
* Algorithm:
* Skip whitespace until the first character.
* Check if the first non-space character is ';'
*/
int is_comment(char *str){
    /* Skip all whitespace at the beginning of the line */
    while(*str!='\0' && isspace(*str)!=ERROR_F){
        str++;
    }
    /* Check if the first character found is a semicolon */
    if(*str==';'){
        return SUCCESS_F; 
    }else{
        return ERROR_F; 
    }
}
/*
* The function creates a new file name with a requested extension.
* Assumptions: The extension is correct.
* Algorithm:
* Allocate memory for the new name  with the extension.
* Copy the original name and remove the ".as" extension.
* Add the new extension.
*/
char *make_new_name_file(char *name_file, char *ending){
    int len;
    char *new_name_file;
    /*Allocating memory for the new file name*/
    new_name_file=(char *)malloc(strlen(name_file)+strlen(ending)+1);
    if(new_name_file==NULL){
        return NULL;/* Check that the allocation was successful */
    }
    strcpy(new_name_file,name_file);
    len=(int)strlen(new_name_file);

    if(len>=LEN_AS && strcmp(new_name_file+ len-LEN_AS,".as") == 0){
        new_name_file[len-LEN_AS] ='\0';/*Remove ".as"*/
    } 
    strcat(new_name_file,ending);/* Add the new extension */
    
    return new_name_file;
}
/*
* The function checks the validity of a label or macro name and prints an error if necessary.
* Algorithm:
* Checks that the name is not empty and does not exceed 31 characters.
* Checks that the first character is an English letter.
* Checks if all the characters are valid and in the macro also allowe '_'.
* Checks that the name is not a reserved word in the language.
*/
int is_valide_name(char *name,int line_number,char *file_name, int is_mac){
    int i;
    /*Array of all words stored in the language */
    static const char *reserved_words[]={
        "add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo",
        "addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt",
        "lb", "sb", "lw", "sw", "lh", "sh",
        "jmp", "la", "call", "hlt",
        ".db", ".dw", ".dh", ".asciz", ".entry", ".extern",
        "mcro", "mcroend",NULL
    };
    /*Check if the name exists and is not empty */
    if(name==NULL || strlen(name)==NO_CHAR){
        fprintf(stderr,"There is an error in file %s at line %d, name is missing.\n",file_name, line_number );
        return ERROR_F;}
    /*Check if the name no exceed 31 characters */
    if(strlen(name)>MAX_M_L_CHAR){
        fprintf(stderr,"There is an error in file %s at line %d,the name '%s' more then 31 characters.\n",file_name,line_number,name);
        return ERROR_F;
    }
    /*Check if the first character is alphabetic letter */
    if(!isalpha(name[FIRST_INDEX])){
        fprintf(stderr,"There is an error in file %s at line %d, the name '%s' must start with an alphabetic letter.\n",file_name,line_number,name);
        return ERROR_F;
    }

    /*Check the validity of the all characters in the name*/
    for(i=1; name[i]!='\0'; i++){
        if(is_mac==IS_MACRO){
            /*Letters,numbers,and '_' are allow in the macro*/
            if(!isalnum(name[i]) && name[i]!='_') {
                fprintf(stderr,"There is an error in file %s at line %d, the name '%s' with invalid characters.\n",file_name,line_number,name);
                return ERROR_F;
            }
        }else{
            /*Only letters and numbers are allow in the label */
            if(!isalnum(name[i])){
                fprintf(stderr,"There is an error in file %s at line %d, the name '%s' with invalid characters.\n",file_name,line_number,name);
                return ERROR_F;
            }

        }
    }
    /*check  that the name is not a reserved word */
    for(i = 0; reserved_words[i]!=NULL; i++){
        if(strcmp(name,reserved_words[i])==SAME){
            fprintf(stderr,"There is an error in file %s at line %d, the name '%s' is a reserved word.\n",file_name,line_number,name);
            return ERROR_F;
        }
    }

 return SUCCESS_F;

}
