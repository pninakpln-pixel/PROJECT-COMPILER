
#include "table.h"
#include "second_pass.h"
#include "helpers.h"
#include "globals.h"

/*
*The second_pass function performs the second pass of the assembler, and completing the binary code.
*The program scans the .am file again, and updates the address fields of the jump (J) and branch (I) instructions in a code image
* according to the symbol table.In addition, it locates .entry instructions and update the list.
* Assumptions:
 *The first pass completed without errors and all labels exist in the symbol table.
*Algorithm how the function works:
*1. We read the .am file line by line (the instruction counter starts at 100).
*2. Skip empty lines, comments, and label definitions.
*3. If there is '.entry' word,we find the label in the table, update is_entry and add to the ent_list.
*4. If there is a J (jump) command find the address of the label and enter it into the code,
* if it is external (extern), write 0 and add it to the extern list.
*5.If there is an I command (branch) - calculate the distance to the label and enter it into the code.
*/
 
int second_pass(char *file_name,Symbol *symbol_list,unsigned char *code_img,EXT_ENT_NODE **ext_list, EXT_ENT_NODE **ent_list){
FILE *am_file;/* The file we are going to read from */   
int line_number=START_VALUE;/* Count what line we are on so we can print errors */
int ic=IC_START_VALUE;/* Instruction counter, starting from 100 as required*/
int error_found=START_VALUE;/* A flag that says if we found an error, no error, this is 0*/
char *ptr_line;/* Variable that runs on the line */
char line[MAX_LINE_LENGTH];/* The array that holds the entire line we read from the file */
char target[MAX_LABEL_LENGTH];/* Holds the target label name*/
char word[MAX_WORD_LENGTH];/* Holds the word we are currently checking */
Symbol *current_symbol=NULL;/* Pointer to find a label from the symbol table */
int index=START_VALUE;/*The location in the machine code array*/
unsigned int address_target=START_VALUE;/* The address of the destination to jump to */
 const Instruction *inst=NULL;/* Pointer to the current instruction data from the instruction table */
int dis=START_VALUE;/* The distance we need to jump (for branch commands) */
/* Try to open the file. If it can't, print an error */
am_file=fopen(file_name,"r");

if(am_file==NULL){
    fprintf(stderr,"there is an error,the file %s cannot open in the second pass.\n.",file_name);
    return ERROR_F;
}
/* Read the file line by line until it ends */
while(fgets(line, sizeof(line),am_file) != NULL){
    line_number++;
    /*If this is a empty line or comment-Skipping*/
    if(is_empty_line(line)||is_comment(line)){
        continue;
    }
    /*Put the variable at the beginning of the line and take the first word */
    ptr_line=line;
    ptr_line=extract_word(ptr_line,word,IS_NOT_REGISTER);
    /* If the first word is a label, skip it */
    if (is_label(word)==SUCCESS_F) {
        ptr_line= extract_word(ptr_line, word,IS_NOT_REGISTER );/* Take the next word */
        if (word[FIRST_INDEX] =='\0') {
            continue; /* If there is nothing after the label, continue to the next line */
        }
    }
    if(word[FIRST_INDEX]=='.'){
        /*If it's .entry we deal with it in the second pass*/
        if(strcmp(word, ".entry")==SAME){
            ptr_line=extract_word(ptr_line,word,IS_NOT_REGISTER );
            /* If the label name does not exist */
            if(word[FIRST_INDEX]=='\0'){
                error_found=ON;
                fprintf(stderr,"there is an error,in file %s on line %d missing label\n",file_name,line_number);
                continue;
            }
            /* Search for the label in the table to see if it exists in the code */
            current_symbol=find_symbol(symbol_list,word);
            if(current_symbol==NULL){
                error_found=ON;
                fprintf(stderr,"there is an error,in file %s on line %d the label %s doesn't exist\n",file_name,line_number,word);
            }else if(current_symbol->type==SYMBOL_EXTERN){
                /* A label cannot be both extern and entry */
                fprintf(stderr,"there is an error,in file %s on line %d the label %s can't be both entry and extern.\n",file_name,line_number,word);
                error_found=ON;

            }else{
                /* If there are no errors, add the label to the entry list and change its status */
                if(add_ext_ent(ent_list,current_symbol->name,current_symbol->address)==MEMORY_ERROR){
                    fprintf(stderr,"there is an error,in file %s on line %d memory allocation failed for new entry node: %s.\n",file_name,line_number,current_symbol->name);
                    
                    if (am_file!= NULL) {
                        fclose(am_file);
                    }
                    return MEMORY_ERROR;/* Return -1 as a memory error indicator */
                } 
                current_symbol->is_entry=IS_ENTRY;
            }
        }
        continue;
    }
    /* This is a normal command */
    inst=find_instruction(word);
    if(inst == NULL){
        fprintf(stderr,"there is an error,in file %s on line %d the %s instruction is unknown\n",file_name,line_number,word);
        error_found=ON;
        continue;
    }
    /* If this is a jump command (J) */
    if(inst->type == TYPE_J_JUMP || inst->type==TYPE_J_LOAD_ADD || inst->type==TYPE_J_CALL){
        
        ptr_line=extract_word(ptr_line,target,IS_NOT_REGISTER );/* Get the label to jump to */
        /* If the label is missing */
        if(target[FIRST_INDEX]=='\0'){
            error_found=ON;
            fprintf(stderr,"there is an error,in file %s on line %d missing operand for %s instruction\n",file_name,line_number,inst->name);
            ic+=BYTES_PER_WORD;
            continue;
        }
        /* Jump to address within a register already handled in the first pass */
        if(target[FIRST_INDEX]=='$'&&strcmp(inst->name,"jmp")==SAME) {
            ic+=BYTES_PER_WORD;
            continue;
        }
        /* Search for the label we want to jump to within the table */
        current_symbol=find_symbol(symbol_list,target);
        if(current_symbol==NULL){
            fprintf(stderr,"there is an error in file %s on line %d, the label %s doesn't exist\n",file_name,line_number,target);
            error_found=ON;
            ic+=BYTES_PER_WORD;
            continue;
        }else{
            /*If found, calculate the position in the machine code array */
            index=(ic-IC_START_VALUE); 
            address_target=current_symbol->address;
            if(current_symbol->type==SYMBOL_EXTERN){
                /* If a label is in another file (extern) then we put 0 */
                address_target=EXTERN_ADDRESS;
                /*Write to the external file*/
                if(add_ext_ent(ext_list,current_symbol->name,ic)==MEMORY_ERROR){
                    fprintf(stderr,"there is an error,in file %s on line %d memory allocation failed for new extern node: %s.\n",file_name,line_number,current_symbol->name);
                   
                    if (am_file != NULL) {
                        fclose(am_file);
                    }
                    return MEMORY_ERROR;/* Return -1 as a memory error indicator */

                }
            }/*Entering the address in machine code, split the 25 bits of the address into 4 bytes in Little-endian order */
            code_img[index] |=(address_target &MASK_OF_BYTE); /* 8 low bits */
            code_img[index+SECOND_BYTE] |=((address_target >> ONE_BYTE) & MASK_OF_BYTE);  /* Next 8 bits */
            code_img[index+THIRD_BYTE] |=((address_target >> TWO_BYTES) & MASK_OF_BYTE); /* Next 8 bits */
            code_img[index+FOURTH_BYTE] |=((address_target >> THREE_BYTES) & MASK_for_SINGLE_BIT); /* The 25th bit goes into the lower bit of the fourth byte */
        }
        ic+=BYTES_PER_WORD;
/* Each command weighs 4 bytes */
    } else if(inst->type==TYPE_I_BRANCH){/* If this is a type I conditional branch */
        /* Extract the first register and skip the comma after it */
        ptr_line=extract_word(ptr_line,word,IS_REGISTER);
        while (isspace(*ptr_line)){
            ptr_line++;
        }
        if(*ptr_line==','){
             ptr_line++;
        }
        /* Extract the second register and skip the comma after it */
        ptr_line=extract_word(ptr_line,word,IS_REGISTER);
          while (isspace(*ptr_line)){
            ptr_line++;
        }
        if(*ptr_line==','){
             ptr_line++;
        }
        /* Take a label and extract it all*/
        ptr_line=extract_word(ptr_line,target,IS_NOT_REGISTER );

        if(target[0]=='\0'){ 
            fprintf(stderr,"there is an error,in file %s on line %d missing label operand for %s instruction\n",file_name,line_number,inst->name);
            error_found=ON;
            ic+=BYTES_PER_WORD;
            continue;
        }
        /* Check if the label exists in the table */
        current_symbol = find_symbol(symbol_list, target);
        if(current_symbol==NULL){
            fprintf(stderr,"there is an error in file %s on line %d, the label %s doesn't exist\n",file_name,line_number,target);
            ic+=BYTES_PER_WORD;
            error_found=ON;
            continue;
        }else if(current_symbol->type == SYMBOL_EXTERN){
            /* Do not branch to an external label */
            fprintf(stderr,"there is an error in file %s on line %d,branch target %s cannot be  external\n",file_name,line_number,target);
            ic+=BYTES_PER_WORD;
            error_found=ON;
            continue;
        }else{
            /* Calculate how many bytes to go forward/backward from the current point*/
            index = (ic-IC_START_VALUE);
            dis=current_symbol->address-ic;
            /* Enter the distance */
            /*Split the 16 bits of the distance into the first 2 bytes*/
            code_img[index] |=(dis&MASK_OF_BYTE);
            code_img[index+SECOND_BYTE] |=((dis>>ONE_BYTE)&MASK_OF_BYTE);
            ic+=BYTES_PER_WORD;
            continue;
        }
    }else{/* The other commands have already been processed in the first pass */
        ic+=BYTES_PER_WORD;
    }   
}
fclose(am_file);
if(error_found==ON){
    return ERROR_F; /*If we found at least one error along the way - we will return 0.*/
}else{
    return SUCCESS_F; /* No errors here, return 1 */
}
}

