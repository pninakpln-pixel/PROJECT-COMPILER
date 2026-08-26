
#include "macro.h"
#include "pre_assembler.h"
#include "helpers.h"
#include "globals.h"
/*
*This file has the pre-assembler.
*Purpose of the file: 
*Reading the assembly program source file (.as), identifying and unpacking all macros,
*and creating an extended source file (.am) that will be passed to the next stage of the assembler.
*Description the algorithm:
*The program opens the file and reads it line by line.
*When there is a keyword "mcro",the program saves the following lines to a data structure until "mcroend" is found.
*When a call to the name of the save macro,the program writes all the saved lines into the output file.
*Input/Output: Receives the name of the source file with the extension .as, and creates an output file with the extension .am .
*Assumptions:
*There are no nested macro definitions.
*each macro is defined before it is called in the code.
*we get a file with an .as extension.
*/


/*
*The function manages the pre-assembler process.
*Opens files, reads lines, identifies macros,saves them, parses them to the output file,
*and finally checks for duplicates of label names with macros.
*Algorithm:
*1. Open input file (.as) and output file (.am).
*2. Read lines in a loop. Check for length exception.
*3. Extract words from the line for testing.
*4.When we see "mcro": Turn on the flag and saving the following lines to a new node in a linked list.
*5.When we see "mcroend": Turn off the flag and ending the collection of lines.
*6.If we are in flag off and we see known macro in the macro list-writing its contents to the output file.
*7.If not - copying the line as it is.
*8.Second pass: Reopen the file to make sure there are no labels that are identical to the name of an existing macro in the list.
*/
int pre_assembler(char *name_file){
    char first_line_word[MAX_WORD_LENGTH];/*Holds the first word(command, label, or macro name)*/
    char second_line_word[MAX_WORD_LENGTH];/*Holds the second word*/
    char extra_word[MAX_WORD_LENGTH];/*Holds the third word to make sure there is no unnecessary text*/
    char l_name[MAX_LABEL_LENGTH];/*Holds the label name*/
    char line[MAX_LINE_LENGTH];/* Holds the line read from the file in each iteration */
    NODE_MACRO *macro_head_list = NULL; /* Pointer to the top of the macro list */
    NODE_MACRO *current_data_line=NULL;/* Pointer to the active macro to which we attach lines of code */
    NODE_MACRO *found_macro = NULL; /* Pointer to get a search result when searching for a macro */
    int len_name;/*holds the length of the found label name */
    int length;/* Saves the current line length */
    int c;/* Helps clean up in case of a line that is too long */
    int is_macro=START_VALUE;/* Boolean flag, 1 if we are inside a macro definition, 0 if not */
    int error_found=START_VALUE;/* Boolean flag, 1 if an error was found, 0 if not */
    int line_number=START_VALUE;/*Counts lines in the file to print error messages*/
    FILE *file_as=NULL;/* Source file to read */
    FILE *file_am = NULL;/* Destination file for writing */
    FILE *file_as2=NULL;/* Pointer to reread the source file */
    char *current_ptr=NULL;/* Passing pointer of the line */
    char *filename_am = NULL;/*Pointer to a string that holds the name of the output file*/
    char temp_word[MAX_WORD_LENGTH];/*Holds the current word in the current line under test*/
    char *ptr=NULL;/*A pointer that runs on the current line.*/


    file_as= fopen(name_file,"r");/*Try to open the source file for reading*/

    if(file_as==NULL){
        fprintf(stderr,"error, unable to open the file %s.\n", name_file);
        return ERROR_F;
    }

    filename_am= make_new_name_file(name_file,".am");/* Create output file name */

    if (filename_am == NULL) {
        fprintf(stderr,"error in file %s at line %d, memory allocation failed for .am name file \n",name_file, line_number);
        fclose(file_as);
        exit(ERROR_EXIT);
    }
    /*Try to open the output file for writing */
    file_am = fopen(filename_am, "w");

    if(file_am==NULL){
        fprintf(stderr,"error, unable to open the file %s.\n", name_file);
        fclose(file_as);
        free(filename_am);

        return ERROR_F;
    }
    /* Main loop-scanning the file line by line until the file ends*/
    while(fgets(line, sizeof(line),file_as)!=NULL){
        line_number++;
        length=(int)strlen(line);
        /* Check if the line exceeds the maximum allowed length */
        if(length==MAX_LINE_LENGTH-END_OF_STRING_CHAR && line[length-TO_LAST_CHAR] != '\n'){
            fprintf(stderr,"error in file %s at line %d: line more then 80 characters.\n", name_file, line_number);
            error_found=ON;
            /* Clear the rest of the long line so as not to destroy the next line */
            while ((c=fgetc(file_as)) != '\n' && c != EOF);
            continue;
        }
        /* Reset the words before the new line */
        first_line_word[FIRST_INDEX]='\0';
        second_line_word[FIRST_INDEX]='\0';
        extra_word[FIRST_INDEX] = '\0';

        /* Extract the first three words from the line */
        current_ptr= line;
        current_ptr= extract_word(current_ptr, first_line_word,IS_NOT_REGISTER);
        current_ptr= extract_word(current_ptr, second_line_word,IS_NOT_REGISTER);
        current_ptr= extract_word(current_ptr, extra_word,IS_NOT_REGISTER);
        
        /*if the line is empty or has only spaces or a comment line */
        if(first_line_word[FIRST_INDEX]=='\0'||is_comment(line)){
            /* If we are inside a macro definition, we also save empty lines */
            if (is_macro==ON) {
                if(add_line_data(current_data_line, line)==MEMORY_ERROR){
                    fprintf(stderr,"error in file %s at line %d, memory allocation failed for macro %s\n",name_file, line_number,first_line_word); 
                    free_macro(&macro_head_list );
                    if(file_am!=NULL)
                    {
                        fclose(file_am);
                    }
                    if(file_as!=NULL)
                    {
                        fclose(file_as);
                    }
                    if (filename_am != NULL) {
                        free(filename_am);
                    }
                    exit(ERROR_EXIT);
                }
            else{
                /* Outside the macro-copy empty lines as they are to the output file */
                if (fputs(line, file_am) == EOF) 
                    fprintf(stderr,"error writing to file %s\n", filename_am);
                }
                continue;
            }
        }
        /* We are in the process of collecting lines into a macro body */
        if(is_macro==ON){
            /* Detect end of macro definition */
            if(strcmp(first_line_word, "mcroend")==SAME){
                if (second_line_word[0]!='\0') {
                    fprintf(stderr,"error in file %s at line %d, extra text after 'mcroend'\n", name_file, line_number);
                    error_found=ON;
                }
                is_macro=OFF;/* Turn off the flag */
                current_data_line=NULL;
            }else{
                /* Add the current line to the macro content */
                if(add_line_data(current_data_line, line)==MEMORY_ERROR){
                    fprintf(stderr,"error in file %s at line %d, memory allocation failed for macro %s\n",name_file, line_number,second_line_word); 
                    free_macro(&macro_head_list );
                    if(filename_am!=NULL)
                    {
                        fclose(file_am);
                    }
                    if(file_as!=NULL)
                    {
                        fclose(file_as);
                    }
                    if (filename_am != NULL) {
                            
                        free(filename_am);
                    }
                    exit(ERROR_EXIT);
                }
            }
                continue;
        }
        /* Detect the start of a new macro definition */
        if(strcmp(first_line_word,"mcro")==SAME) {
            if(second_line_word[FIRST_INDEX]=='\0') {
                fprintf(stderr,"error in file %s at line %d, missing macro name\n", name_file, line_number);
                    error_found=ON;
            }

            else if (extra_word[FIRST_INDEX]!='\0') {
                    fprintf(stderr,"error in file %s at line %d, extra text after macro name.\n", name_file, line_number);
                    error_found=ON;
            }
            else if (is_valide_name(second_line_word, line_number, name_file,IS_MACRO)==ERROR_F){
                error_found=ON;
            }   
            else if (find_macro(macro_head_list, second_line_word)!=NULL){
                fprintf(stderr,"error in file %s at line %d, two macros with the same name %s.\n",name_file,line_number,second_line_word);
                error_found=ON;
            }else{
                /* The name is valid and exists and there are no duplicates, open a new macro */
                is_macro=ON;
                current_data_line = add_macro(&macro_head_list, second_line_word);
                if (current_data_line == NULL) {
                    fprintf(stderr,"error in file %s at line %d, memory allocation failed for macro %s.\n",name_file,line_number,second_line_word);    
                    free_macro(&macro_head_list );
                    if(filename_am!=NULL)
                    {
                        fclose(file_am);
                    }
                    if(file_as!=NULL)
                    {
                        fclose(file_as);
                    }
                    exit(ERROR_EXIT);
                }
            }
            continue;
        }
        /* Check if the first word is a call to a macro that has already been saved */
        found_macro = find_macro(macro_head_list, first_line_word);   
        if(found_macro != NULL){
            /* Case where the macro appears first, but there is text after it */
            if(second_line_word[FIRST_INDEX]!='\0'){
                fprintf(stderr,"error in file %s at line %d there is an extra text after macro call\n", name_file, line_number);
                error_found=ON;
            }else{   /* Write the macro content to the output file */
                if(found_macro->data!=NULL){
                    fputs(found_macro->data, file_am);
                }
            }

        }else{/* The first word is not a macro. Go through all the words in the line, making sure there is no macro name in the middle of the line. */
                ptr=line;
                ptr = extract_word(ptr,temp_word,IS_NOT_REGISTER);/* Skip the first word we have already checked */
                while(*ptr!='\0'){
                    ptr=extract_word(ptr,temp_word,IS_NOT_REGISTER );
                    if(temp_word[FIRST_INDEX]=='\0'){
                        break;
                    }
                    /* Check that macro does not appear after words in the line*/
                    if(strcmp(temp_word,"mcro")==SAME){
                        fprintf(stderr,"error in file %s at line %d,'mcro' must be in the start of the line.\n", name_file, line_number);
                        error_found=ON;
                        break;
                    }
                    /* Check that macroend does not appear after words in the line */
                    if(strcmp(temp_word,"mcroend")==SAME){
                        fprintf(stderr,"error in file %s at line %d, 'mcroend' must be alone in the line.\n", name_file, line_number);
                        error_found=ON;
                        break;
                    }
                    /*Checking that there is no macro call in the middle of a line*/
                    if(find_macro(macro_head_list,temp_word)!=NULL) {
                        fprintf(stderr,"error in file %s at line %d, macro %s cannot be used in the middle of a line.\n", name_file, line_number, temp_word);
                        error_found=ON;
                        break;
                    }
                }
                /* Normal line of code - copied as is */
                if(fputs(line,file_am)==EOF){
                fprintf(stderr,"error in file %s, there is a problem writing to the file %s the line %d\n",name_file,filename_am,line_number);
                }
            }
        }
        
    
    /* Close files after the  pass */
        fclose(file_as);
        fclose(file_am);
        /* Reopen the source file to check names between labels and macros */
        file_as2= fopen(name_file,"r");
        line_number=START_VALUE;
        if (file_as2 != NULL) {
        while(fgets(line,sizeof(line),file_as2)!=NULL){
                line_number++;
                if (is_empty_line(line) || is_comment(line)){
                continue;
                }

            extract_word(line,l_name,IS_NOT_REGISTER);
            len_name = strlen(l_name);
            /* If the word ends with ':', this is a label definition */
            if (len_name>0 && l_name[len_name-TO_LAST_CHAR ]==':'){
                l_name[len_name-TO_LAST_CHAR]='\0';/* Removing the ':' from the name */
                /* Check if the label name is the same as the name of an existing macro */
                if (find_macro(macro_head_list, l_name) != NULL) {
                    fprintf(stderr,"error label name '%s' on line %d, same as macro name.\n", 
                    l_name, line_number);
                    error_found= ON;
                }

            }

        }
        fclose(file_as2);
        }else{
            fprintf(stderr,"error, unable to reopen the file %s.\n", name_file);
            error_found=ON; 
        }

        /* We will clear the memory and files in case an error is found at any step */
        if (error_found==ON){
            remove(filename_am);/* delete the am file because there is  errors */
            free(filename_am);
            free_macro(&macro_head_list);
            return ERROR_F;
        }
        /* Free memory in case of success */
        free(filename_am);
        free_macro(&macro_head_list);

    return SUCCESS_F;
}









