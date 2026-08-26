
#include "pre_assembler.h"
#include "first_pass.h"  
#include "second_pass.h"
#include "helpers.h"
#include "table.h"
#include "globals.h"
#include "macro.h"



int ICF;/* The final counters of the instructions and data. */
int DCF;

/*
*Purpose of the program:
*The main file of the project that manages the entire process of translating the instructions and data assembly files into machine code, 
*and to produce the output files(am, .ob, .ent, .ext).
*the program works like this:
*The program receives the file names from the command line and processes each file separately according to the following steps
*1. Pre-assembler: reads the .as file, identifies macro definitions,
* parses their lines in the appropriate places, and saves the result in the .am file.
*2. First pass: scans the .am file, checks for syntactic correctness,
* builds the symbol table with initial addresses, and partially encodes the
* instruction image and data image.
*3. Second pass: Completes the binary encoding of the label addresses
* and branches using the symbol table, and builds the entry and extern lists.
*Input:
*File names passed on the command line with the extension ".as".
*Output:
*If no errors are detected creates an object file (.ob) , an entry file (.ent) , and an external file (.ext)
*Frees dynamically allocated linked lists and strings .
*Assumptions :
*The total memory size for instructions and data does not exceed the 4096-byte limit.
*No nested macro settings.
*/


int main(int argc, char *argv[]){
    /* Index variables for loops */
    int i;
    int j;
    int m;
    int k;
    int pass_status;/*The value returned from the second pass*/
    int first_status;/*The value returned from the first pass*/
    char *name_file_am=NULL;/* Name of the expanded source file (.am) */
    char *name_file_ob=NULL;/* Name of the object file ().ob )*/
    char *name_file_ent=NULL;/* Name of the entry labels file (.ent) */
    char *name_file_ext=NULL;/* Name of the external file(.ext) */

    FILE *file_ob = NULL;/* Pointer to object file */
    FILE *file_ent = NULL;/* Pointer to an entry file */
    FILE *file_ext = NULL;/* Pointer to an externals file */

    EXT_ENT_NODE *current = NULL;/* Head of global symbol table list */
    EXT_ENT_NODE *ent_list=NULL;/* Head of entry label list */
    EXT_ENT_NODE *ext_list=NULL;/* Head of extern label list */
    Symbol *symbol_list=NULL;/* Head of symbol table list */
    unsigned char code_img[MEMORY_SIZE];/*array for the code memory */
    char *data_img=NULL;/* array for the data memory */

    int name_len=START_VALUE;/* File name length */
    
    printf("Welcome to the Assembler Program!\n\n");
    /* Check if there are arguments on the command line */
    if(argc <MIN_ARG){
        fprintf(stderr,"There is an error,no input files, please enter files like %s <file1.as>...\n",argv[FIRST_INDEX]);
        return ERROR_MAIN;
    }
    printf("Starting assembler processing, there are %d files to process.\n",argc-NO_FILE_ARG);
   
    /* Main loop: individual processing for each input file */
    for(i=1; i<argc; i++){
        /* Initial reset of the memory image arrays */
        for (m=0; m<MEMORY_SIZE ; m++) {
        code_img[m]=ARRAY_UPDATE;
        }

        name_len=strlen(argv[i]);
        /* Check the file extension as a valid assembly extension (.as) */
        if(name_len<MIN_FILE_NAME_LENGTH || strcmp(argv[i]+name_len-LEN_AS,".as")!= 0) {
        fprintf(stderr,"Error, missing .as ending in file %s.\n", argv[i]);
        continue;
        }
        
        /* Initialize counter values ​​and pointers for processing the current file */
        ICF=IC_START_VALUE ;
        DCF= DC_START_VALUE ;
        name_file_am=NULL;
        ent_list=NULL;
        ext_list=NULL;
        symbol_list=NULL;
        data_img=NULL;

        printf("\nProcessing a file name: %s, at location %d/%d \n",argv[i],i,argc-1);
        printf("Running pre_assembler (Macro Deployment).\n");
        /*Deploy macros and create an extended source file (.am) */
        if(pre_assembler(argv[i])==ERROR_F){
            fprintf(stderr,"There are errors during macro expansion in file %s,skipping this file\n",argv[i]);
            continue;
        }else{
             printf("Pre-assembler completed successfully.\n\n");
        }
        /*Create the .am file name for further processing */
        name_file_am=make_new_name_file(argv[i],".am");
        if(name_file_am==NULL){
            fprintf(stderr,"there is an errors,in file %s memory allocation failed for name_file_am.\n",argv[i]);
            exit(ERROR_EXIT);
        }

        printf("Running first pass.\n");
        first_status=first_pass(name_file_am,&symbol_list,code_img,&data_img);
        if(first_status==ERROR_F||first_status==MEMORY_ERROR){
            fprintf(stderr,"There are errors during first_pass in file %s,skipping this file\n",argv[i]);
            /*free in case of error*/
            if (symbol_list != NULL) {
                free_symbol(&symbol_list);
            }
            if (data_img != NULL) {
                free(data_img);
            }
            /* Final stop if this is a malloc error */
            if(first_status==MEMORY_ERROR) {
                fprintf(stderr,"There is a memory error in file %s. Exiting the program.\n",argv[i]);
                exit(ERROR_EXIT);
            }
            continue; 
        }else{
            printf("First pass completed successfully.\n\n");
        }
        

        /*Run the second pass and complete the memory encodings */
        
        printf("Running second pass.\n");
        pass_status=second_pass(name_file_am,symbol_list,code_img,&ext_list,&ent_list);
        if(pass_status==ERROR_F || pass_status==MEMORY_ERROR){
            fprintf(stderr,"There are errors during second pass in file %s,skipping output file creation\n",argv[i]);
            /*free in case of error*/
            free(name_file_am);
            if(data_img!=NULL){
                free(data_img);
                data_img = NULL;
            }
            if(symbol_list!=NULL){
                free_symbol(&symbol_list);
            }
            if(ent_list!=NULL){
                free_ext_ent(ent_list);
            }
            if(ext_list!=NULL){
                free_ext_ent(ext_list);
            }
            /* Final stop if this is a malloc error */
            if(pass_status==MEMORY_ERROR){
                fprintf(stderr,"There is a memory error in file %s. Exiting the program.\n",argv[i]);
                exit(ERROR_EXIT);
            }
            /* If this is just a syntax error, move to the next file */
            continue;
        }else{ 
            printf("Second pass completed successfully.\n\n");
            printf("creating output files.\n");
            
            /*Create the object file (.ob) */
            name_file_ob=make_new_name_file(argv[i],".ob");
            if (name_file_ob==NULL){
                fprintf(stderr,"There is an erroro, memory allocation failed for .ob file name in file %s.\n",argv[i]);
                exit(ERROR_EXIT);
            }else{
                file_ob=fopen(name_file_ob,"w");
                if(file_ob==NULL){
                    fprintf(stderr,"there is an error, cannot open output file %s.\n",name_file_ob);
                }else{
                    /* Writing a header: the number of instruction and the number of data in memory */
                    fprintf(file_ob,"     %d %d\n",ICF-IC_START_VALUE,DCF);
                    /* Write the instruction image in Little-Endian format (4 bytes per line) */
                    for(j=IC_START_VALUE;j<ICF; j+=BYTES_PER_WORD){
                        int index=j-IC_START_VALUE;
                        fprintf(file_ob,"%04d %02X %02X %02X %02X\n",j,code_img[index],code_img[index+SECOND_BYTE],code_img[index+THIRD_BYTE],code_img[index+FOURTH_BYTE]);
                    }
                    /* Writing the data image up to 4 bytes per line*/
                    for (j=0;j<DCF;j+=BYTES_PER_WORD) {
                        fprintf(file_ob,"%04d",ICF+j);

                        for(k=0; k<BYTES_PER_WORD && (j+k)<DCF; k++) {
                            fprintf(file_ob," %02X",(unsigned int)(data_img[j+k] & 0xFF));
                        }
                        fprintf(file_ob, "\n");
                    }
                    fclose(file_ob);
                }
                free(name_file_ob);
            }
            /*Create an entry file (.ent) if entry labels were defined */
            if(ent_list==NULL){
                printf("There is no output file of type entry ,because there is no entry label.");
            }else{
                name_file_ent=make_new_name_file(argv[i],".ent");
                if(name_file_ent==NULL){
                    fprintf(stderr,"There is an erroro, memory allocation failed for .ent file name in file %s.\n",argv[i]);
                    exit(ERROR_EXIT);
                }else{
                    file_ent=fopen(name_file_ent, "w");
                    if(file_ent == NULL){
                        fprintf(stderr,"there is an error, cannot open output file %s.\n",name_file_ent);
                        
                    }else{
                        current=ent_list;
                        while(current!=NULL){
                            fprintf(file_ent,"%s %04d\n",current->name, current->address);
                            current=current->next;

                        }
                        fclose(file_ent);
                    }
                    free(name_file_ent);
                }
            }
            /*Create an external file (.ext) if extern symbols are used */   
            if(ext_list==NULL){
                printf("There is no output file of type externals ,because there is no extern label.");
            }else{
                name_file_ext=make_new_name_file(argv[i],".ext");
                if(name_file_ext==NULL){
                    fprintf(stderr,"There is an erroro, memory allocation failed for .ext file name in file %s.\n",argv[i]);
                    exit(ERROR_EXIT);
                }else{
                    file_ext=fopen(name_file_ext,"w");
                    if(file_ext == NULL){
                        fprintf(stderr,"there is an error, cannot open output file %s.\n",name_file_ext);            
                    }else{
                        current=ext_list;
                        while(current!=NULL){
                            fprintf(file_ext,"%s %04d\n",current->name,current->address);
                            current=current->next;

                        }
                        fclose(file_ext);
                    }
                    free(name_file_ext);
                }
            }
            printf("Output files created successfully for %s.\n",argv[i]);
            /*Completely free all dynamic memory for the current file */
            free(name_file_am);
            if (data_img != NULL) {
                free(data_img);
                data_img = NULL;
            }
            if (symbol_list != NULL){
                free_symbol(&symbol_list);
            }
            if (ent_list != NULL){
                free_ext_ent(ent_list);
            }
            if (ext_list != NULL){
                free_ext_ent(ext_list);
            }
            printf("Assembly complete for file %s.\n",argv[i]);           
        }
    }
    return SUCCESS_MAIN;
}