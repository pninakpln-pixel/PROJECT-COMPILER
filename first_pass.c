#include "first_pass.h" 

/* 
* This function reads an assembly file line by line tfter the Macro opening, for the first pass. It builds the symbol 
* table, encodes data and instructions, and updates IC and DC counters. 
* 
* Assumptions: 
* - file_name is a valid path to the assembly file. 
* - symbol_head, code_img, and data_img are valid pointers. 
* - the previause compiler level ended successfuly.
* 
* Algorithem: 
* - Opens the file and sets IC and DC counters to start values. 
* - Reads line by line, skipping empty lines and comment lines. 
* - Checks if line starts with a label and saves the label name. 
* - Care data directives (.db, .dh, .dw, .asciz), saves data labels with current DC and code the data into the memmory. 
* - Care entry (.entry) and extern (.extern) directives. 
* - Care instruction lines, saves code labels with current IC, and codes instructions. 
* - Closes file and stops if any error was found. 
* - Updates data symbol addresses by adding total IC (ICF) at the end. 
*/
int first_pass(char *file_name, Symbol **symbol_head, unsigned char *code_img, char **data_img) {
    FILE *am_file; /* File pointer for input file */
    char line[MAX_LINE_LENGTH]; /* Buffer to store the current line */
    int line_num = START_VALUE; /* Line number counter for error messages */
    int error_flag = OFF; /* Flag to show if any error found */
    int ic = IC_START_VALUE; /* Instruction counter */
    int dc = DC_START_VALUE; /* Data counter */
    
    /* Open assembly file for reading */
    am_file = fopen(file_name, "r");
    if (am_file == NULL) {
        fprintf(stderr, "Error: Opening file %s for first pass failed.\n", file_name);
        return ERROR_F;
    }

    /* Read the file line by line */
    while (fgets(line, MAX_LINE_LENGTH, am_file) != NULL) {
        char *line_ptr = line; /* Pointer for the current line */
        char word[MAX_WORD_LENGTH]; /* Buffer to store the current word */
        int exist_label = OFF; /* Flag to check if line has a label */
        char label_name[MAX_WORD_LENGTH]; /* store the label name if exist */
        int process_instruction_status;

        /* Remove newline character from end of line */
        line[strlen(line) - 1] = '\0';
        line_num++;
        
        /* Skip empty lines and comment lines */
        if (is_comment(line_ptr) || is_empty_line(line_ptr)) {
            continue;
        }

        /* Check if the line starts with a label */
        line_ptr = extract_word(line_ptr, word, IS_NOT_REGISTER);
        if (is_label(word)) {
            exist_label = ON;
            word[strlen(word) - 1] = '\0';  /* Remove ':' from end of the label name */
            strcpy(label_name ,word);

            /* Ensure there is content after the label */
            if (is_comment(line_ptr) || is_empty_line(line_ptr)) {
                fprintf(stderr, "Error at file: %s, line %d: Sould be eny instruction after lable\n", file_name, line_num);
                error_flag = ON;
                continue;
            }
            /* Read next word after label */
            line_ptr = extract_word(line_ptr, word, IS_NOT_REGISTER);
        }

        /* Care data and extern/entry directives that starting with '.' */
        if (word[FIRST_INDEX] == '.') {
            if (!strcmp(word, ".db") || !strcmp(word, ".dw") || !strcmp(word, ".dh") || !strcmp(word, ".asciz")) {
                /* Add label into symbol table as data type with current DC */
                if (exist_label) {
                    int add_symbol_status = add_symbol(symbol_head, label_name, dc, SYMBOL_DATA, line_num, file_name);
                    if (add_symbol_status == MEMORY_ERROR) return MEMORY_ERROR;
                    if (!add_symbol_status) error_flag = ON;
                }
                /* Encode string or numbers into data memory image with increasing DC */
                if (strcmp(word, ".asciz") == 0) {
                    int extract_copy_asciz_data_state = extract_copy_asciz_data (line_ptr, data_img, &dc, line_num, file_name);
                    if (extract_copy_asciz_data_state == MEMORY_ERROR) return MEMORY_ERROR;
                    if (!extract_copy_asciz_data_state) error_flag = ON;
                } else {
                    long temp_arr[MAX_LINE_LENGTH] = {START_VALUE};
                    int count = START_VALUE, bytes_per_val;
                    int copy_num_data_status;

                    /* Set byte size for each data type */
                    if (strcmp(word, ".db") == 0) bytes_per_val = BYTES_PER_BYTE;
                    else if (strcmp(word, ".dw") == 0) bytes_per_val = BYTES_PER_WORD;
                    else if (strcmp(word, ".dh") == 0) bytes_per_val = BYTES_PER_HALF_WORD;
                    if (!extract_numbers_data(line_ptr, temp_arr, &count, line_num, file_name)) error_flag = ON;

                    copy_num_data_status = copy_num_data(data_img, &dc, temp_arr, count, bytes_per_val, line_num, file_name);
                    if (copy_num_data_status == MEMORY_ERROR) return MEMORY_ERROR;
                    if (!copy_num_data_status) error_flag = ON;
                }
                continue; 

            } else if (!strcmp(word, ".entry") || !strcmp(word, ".extern")) {
                
                /* skip .entry directive, the scond pass will care it */
                if (strcmp(word, ".entry") == 0) {
                    if (exist_label)
                        fprintf(stderr, "Warning for file: %s, line %d: A label defined at the beginning of a '.entry' line is meaningless", file_name, line_num);
                    line_ptr = extract_word(line_ptr, word, IS_NOT_REGISTER);/*care lable operand later*/

                    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
                        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line: \"%s\"\n", file_name, line_num, line_ptr);
                        error_flag = ON;
                    }                    
                    continue;
                }
                
                /* Handle .extern directive: Add label to symbol table as external label with address 0 */
                if (strcmp(word, ".extern") == 0) {
                    int add_symbol_status;
                    
                    if (exist_label)
                      fprintf(stderr, "Warning for file: %s, line %d: A label defined at the beginning of a '.extern' line is meaningless", file_name, line_num);
                
                    line_ptr = extract_word(line_ptr, label_name, IS_NOT_REGISTER);
                    
                    add_symbol_status = add_symbol(symbol_head, label_name, 0, SYMBOL_EXTERN, line_num, file_name);
                    if (add_symbol_status == MEMORY_ERROR) return MEMORY_ERROR;
                    if (!add_symbol_status) error_flag = ON;

                    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
                        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line: \"%s\"\n", file_name, line_num, line_ptr);
                        error_flag = ON;
                    }
                    continue;
                }
            }
            else {
                /* Error the directive is unknown  */
                fprintf(stderr, "Error at file: %s, line %d: Unknown instruction '%s'\n", file_name, line_num, word);
                error_flag = ON;
                continue;
            } 
        }  
        /* This is a code line, save label if exist to symbol table as code type with current IC */
        if (exist_label) {
            int add_symbol_status = add_symbol(symbol_head, label_name, ic, SYMBOL_CODE, line_num, file_name);
            if (add_symbol_status == MEMORY_ERROR) return MEMORY_ERROR;
            if (!add_symbol_status) error_flag = ON;    
        }
       
        /* Process instruction and write the coded word to code memory image */
        process_instruction_status = process_instruction(word, line_ptr, code_img, &ic, line_num, file_name);
        if (process_instruction_status == MEMORY_ERROR) return MEMORY_ERROR;
        if (!process_instruction_status) error_flag = ON;
    }
    
    /* Close file after reading all lines */
    fclose(am_file);
    /* Stop if any error founded */
    if (error_flag) return ERROR_F;
    /* Save final IC and DC values */
    ICF = ic;
    DCF = dc;
    /* Add ICF value to all data symbol addresses */
    update_data_symbols_address(*symbol_head, ic);

    return SUCCESS_F;
}

/*
 * This function reallocates the memory buffer reserved for the data image to store additional sata.
 *
 * Assumptions:
 * - data_img is a valid pointer to a dynamically allocated buffer (or NULL initially).
 * - dc represents the current size of the data image in bytes.
 * - bytes_to_add is a positive number of bytes to allocate.
 *
 * Algorithm:
 * Resizes the data buffer using realloc to fit the new total byte count, 
 * updates the pointer and return 1 if success, or -1 if memory allocation failure.
 */
int make_data_space(char **data_img, int dc, int bytes_to_add, int line_number, char *file_name) {
    char *temp;

    if ((dc+bytes_to_add) > MAX_DATA_IMAGE_SIZE) {
        fprintf(stderr, "Error at file: %s, line %d: No more space in the data image.\n", file_name, line_number);
        return MEMORY_ERROR;
    }
    
    /* Attempt to make memory allocation for the new data bytes */
    temp = (char *)realloc(*data_img, dc + bytes_to_add);
    
    /* Check whether memory allocation succeeded */
    if (temp == NULL) {
        fprintf(stderr, "Error at file: %s, line %d: Memory allocation for data image failed!\n", file_name, line_number);
        return MEMORY_ERROR;
    }
    
    /* Update the original pointer to the new memory address */
    *data_img = temp;
    return SUCCESS_F;
}


int extract_numbers_data(char *line_ptr, long *values_out, int *count_out, int line_number, char *file_name) {
    char *endptr;
    long val;

    *count_out = START_VALUE; 

    /* Check if there is no numbers */
    if (is_comment(line_ptr) || is_empty_line(line_ptr)){
        fprintf(stderr, "Error at file: %s, line %d: Missing number argument for data directive.\n", file_name, line_number);
        return ERROR_F;
    }

    /* Loop to process all numbers in the line */
    while (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {

        /* Skip leading whitespaces */
        while (isspace(*line_ptr)) line_ptr++;

        /* 1. Check for unexpected comma */
        if (*line_ptr == ',') {
            fprintf(stderr, "Error at file: %s, line %d: Unexpected ',' found before \"%s\".\n", file_name, line_number, line_ptr);
            return ERROR_F;
        }

        /* Extract the next number */
        val = strtol(line_ptr, &endptr, REGULAR_BASE);
        if (line_ptr == endptr) {
            fprintf(stderr, "Error at file: %s, line %d: Invalid numeric token at '%s', or trailing comma at end of line.\n", file_name, line_number, line_ptr); /*!!!!!!!!!!*/
            return ERROR_F;
        }

        /* Store the number if valid */
        values_out[(*count_out)++] = val;
        line_ptr = endptr;

        if (is_comment(line_ptr) ||is_empty_line(line_ptr)) break;

        /* Skip whitespace and ensure exists comma between numbers */
        while (isspace(*line_ptr)) line_ptr++;       
        if (*line_ptr != ',' && !is_comment(line_ptr) && !is_empty_line(line_ptr)) {
            fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
            return ERROR_F;
        }
        line_ptr++; /* Skip thecomma */
    }
    return SUCCESS_F;
}

/*
 * This function codes an array of numeric values into the data memory image in Little-Endian format.
 *
 * Assumptions:
 * - values contains valid numerical values.
 * - bytes_per_val is valid (1 for byte, 2 for half-word, 4 for word).
 * - dc points to the current data counter index.
 *
 * Algorithm:
 * Calculates total byte size required, allocates memory using make_data_space, 
 * and extracts each byte of each value in Little-Endian order into the data image while updating dc.
 */
int copy_num_data(char **data_img, int *dc, long *values, int count, int bytes_per_val, int line_number, char *file_name) {
    int total_bytes;
    int i, b;

    /* Calculate total bytes required for all numbers */
    total_bytes = count * bytes_per_val;
    
    /* Ensure that memory allocation in the data image is successful*/
    if (make_data_space(data_img, *dc, total_bytes, line_number, file_name) == MEMORY_ERROR) return MEMORY_ERROR;

    /*Go through each number and store its bytes in Little-Endian order */
    for (i = 0; i < count; i++) {
        long val = values[i];

        /* 3. Check number range according to byte size */
        if (bytes_per_val == BYTES_PER_BYTE && (val < MIN_BYTE_VAL || val > MAX_BYTE_VAL)) {
            fprintf(stderr, "Error at file: %s, line %d: Number %ld out of byte range.\n", file_name, line_number, val);
            return ERROR_F;
        }
        if (bytes_per_val == BYTES_PER_HALF_WORD && (val < MIN_HALF_WORD_VAL || val > MAX_HALF_WORD_VAL)) {
            fprintf(stderr, "Error at file: %s, line %d: Number %ld out of half-word range.\n", file_name, line_number, val);
            return ERROR_F;
        }
        if (bytes_per_val == BYTES_PER_WORD && (val < MIN_WORD_VAL || val > MAX_WORD_VAL)) {
            fprintf(stderr, "Error at file: %s, line %d: Number %ld out of word range.\n", file_name, line_number, val);
            return ERROR_F;
        }

        /* Extract each byte using bit shifting and mask */
        for (b = 0; b < bytes_per_val; b++) {
            (*data_img)[*dc] = (char)((val >> (b * BITS_PER_BYTE)) & 0xFF);
            (*dc)++;
        }
    }

    return SUCCESS_F;
}

/*
 *This function process a string operand with double quotes from an .asciz directive and writes its characters followed by a null-terminator into the data image.
 *
 * Assumptions:
 * - line_str points to the argument string following the .asciz directive.
 * - dc points to the current data counter index.
 *
 * Algorithm:
 * Skips whitespace, checks opening quote exist, copies characters to the data image, adds the null terminator '\0', and checks that no invalid trailing characters exist.
 */
int extract_copy_asciz_data (char *line_str, char **data_img, int *dc, int line_number, char *file_name) {
    /* Store first DC position to restore it if something wrong */
    int start_dc = *dc;

    if (is_comment(line_str) || is_empty_line(line_str)){
        fprintf(stderr, "Error at file: %s, line %d: Missing string argument for .asciz directive.\n", file_name, line_number);
        return ERROR_F;
    }
    /* Skip whitespaces from the start of the line */
    while (isspace(*line_str)) line_str++;

    /* Make sure the string starts with a double quote */
    if (*line_str != '"') {
        fprintf(stderr, "Error at file: %s, line %d: String must start with \".\n", file_name, line_number);
        return ERROR_F; 
    }
    line_str++; 
    
    /* Ensure that memory allocation in the data image is successful*/
    if (make_data_space(data_img, *dc, MAX_LINE_LENGTH, line_number, file_name) == MEMORY_ERROR) return MEMORY_ERROR;
    
    /* Copy characters to data image until closing quote or end of string */
    while (*line_str != '"' && *line_str != '\0')
        (*data_img)[(*dc)++] = *line_str++;
    /* Make sure string ends with a double quote */
    if (*line_str != '"') {
        fprintf(stderr, "Error at file: %s, line %d: String must end with \".\n", file_name, line_number);
        *dc = start_dc;
        return ERROR_F; 
    }
    line_str++;
    /* Put null-terminator byte to complete the string */
    (*data_img)[(*dc)++] = '\0';

    /* Make sure no extra trailing characters exist after the string */
    if (!is_comment(line_str) && !is_empty_line(line_str)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line: \"%s\"\n", file_name, line_number, line_str);
        *dc = start_dc;
        return ERROR_F;
    }

    return SUCCESS_F;
}

/*
 * The three folowing Functions: build_r_word, build_i_word, build_j_word constructs a 32-bit machine code instruction word 
 * for R, I, or J format instructions by packing opcode, register numbers, and immediate/address values into their appropriate bit fields.
 *
 * Assumptions:
 * - All instruction field parameters are valid and fit within their specified bit widths.
 *
 * Algorithm:
 * Shifts each field value to its bit offset according to the instruction template,
 * masks immediate/address values to their exact bit range, and merges them using bitwise OR operations.
 */
long build_r_word(int opcode, int rs, int rt, int rd, int funct) {
    long word = START_VALUE;

    word |= (opcode << 26); /* Pack opcode into bits 26-31 */
    word |= (rs << 21); /* Pack rs register into bits 21-25 */
    word |= (rt << 16); /* Pack rt register into bits 16-20 */
    word |= (rd << 11); /* Pack rd register into bits 11-15 */
    word |= (funct << 6); /* Pack funct field into bits 6-10 */

    return word;
}
long build_i_word(int opcode, int rs, int rt, int immed) {
    long word = START_VALUE;
    
    word |= (opcode << 26); /* Pack opcode into bits 26-31 */
    word |= (rs << 21); /* Pack rs register into bits 21-25 */
    word |= (rt << 16); /* Pack rt register into bits 16-20 */
    word |= (immed & 0xFFFF); /* Mask and pack 16-bit immediate into bits 0-15 */
    
    return word;
}
long build_j_word(int opcode, int reg, int address) {
    long word = START_VALUE;

    word |= (opcode << 26); /* Pack opcode into bits 26-31 */
    word |= (reg << 25); /* Pack reg flag into bit 25 */
    word |= (address & 0x1FFFFFF); /* Mask and pack 25-bit address into bits 0-24 */
    
    return word;
}

/*
 * This function processes and validates a register string (for example, "$5") and converts it to its integer index.
 *
 * Assumptions:
 * - str is a non-null string containing a candidate register token.
 *
 * Algorithm:
 * Ensure the leading '$' symbol, found numerical register index using strtol,
 * also ensures no trailing invalid characters remain, and checks Range value between 0 and 31
 * returns the register number if all right, or -1 else.
 */
int register_num (char *str, int line_number, char *file_name) {
    char *endptr;
    int reg_num;

    /* Make sure register string starts with '$' */
    if (*str != '$') {
        fprintf(stderr, "Error at file: %s, line %d: Expected '$' for register: '%s'.\n", file_name, line_number, str);
        return -1;
    }
    str++; 

    /* Extracts the numerical register index */
    reg_num = (int)strtol(str, &endptr, 10);
    /* Ensure digits were converted and no invalid characters exist after the register number */
    if (str == endptr || *endptr != '\0') {
        fprintf(stderr, "Error at file: %s, line %d: Invalid register number '%s'.\n", file_name, line_number, str);
        return -1;
    }
    /* Ensure register number is in valid range (0-31) */
    if (reg_num < 0 || reg_num > 31) {
        fprintf(stderr, "Error at file: %s, line %d: Register number $%d out of range (0-31).\n", file_name, line_number, reg_num);
        return -1;
    }
    
    return reg_num;
}

/*
 * This function analyzes and validates a 16-bit integer immediate value from a string.
 *
 * Assumptions:
 * - str is a non-null string containing a candidate immediate operand.
 *
 * Algorithm:
 * Converts the string to an integer using strtol, ensures no invalid  
 * characters exist after then, and checks the value range 16-bit
 * returns the immediate value if all right, or WRONG_IMMED else.
 */
int immediate_to_num(char *str, int line_number, char *file_name) {
    char *endptr;
    int val;

    /* Extracts the numerical value from string */
    val = (int)strtol(str, &endptr, 10);
    /* Ensure digits were converted and no invalid characters exist after the number */
    if (str == endptr || *endptr != '\0') {
        fprintf(stderr, "Error at file: %s, line %d: Invalid immediate number '%s'.\n", file_name, line_number, str);
        return WRONG_IMMED; /* המחרוזת אינה מספר טהור (למשל תווית) */
    }

    /* Ensure immediate value is within 16-bit integer range */
    if (val < -32768 || val > 32767) {
        fprintf(stderr, "Error at file: %s, line %d: Immediate value %d out of 16-bit range.\n", file_name, line_number, val);
        return WRONG_IMMED;
    }

    return val;
}

/*
 * This Function analyzes register operands for R-type assembly instructions and builds the binary word.
 *
 * Assumptions:
 * - instr points to a valid R-type instruction structure.
 * - line_ptr points to the remaining operand text of the line.
 *
 * Algorithm:
 * Extracts rs, rt (if applicable), and rd registers, validates commas and operand 
 * checks end-of-line content, and build the 32-bit mechine instruction word.
 */
int process_r_instruction(const Instruction *instr, char *line_ptr, long *coded_word, int line_number, char *file_name) {
    char token_reg[MAX_LINE_LENGTH];
    int rs = START_VALUE, rt = START_VALUE, rd = START_VALUE;

    if(is_comment(line_ptr) || is_empty_line(line_ptr)){
        fprintf(stderr, "Error at file: %s, line %d: Missing operands for instruction.\n", file_name, line_number);
        return ERROR_F;
    }
    /* Extract rs register */
    line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
        if (token_reg[FIRST_INDEX] == '\0') {
            fprintf(stderr, "Error at file: %s, line %d: Unexpected ',' before operands.\n", file_name, line_number);
            return ERROR_F;
        }
    if ((rs = register_num(token_reg, line_number, file_name)) == -1) return ERROR_F;

    /* Skip whitespace and check comma after rs */
    while (isspace(*line_ptr)) line_ptr++;       
    if (*line_ptr != ',') {
        fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
        return ERROR_F;
    }
    line_ptr++;

    /* Extract rt register for ALU instructions */
    if (instr->type == TYPE_R_ALU) {
        line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
        if (token_reg[FIRST_INDEX] == '\0') {
            fprintf(stderr, "Error at file: %s, line %d: Too much ',' between operands, Or missing operands.\n", file_name, line_number);
            return ERROR_F;
        }
        if ((rt = register_num(token_reg, line_number, file_name)) == -1) return ERROR_F;

        /* Skip whitespace and check comma after rt */
        while (isspace(*line_ptr)) line_ptr++;   
        if (*line_ptr != ',') {
            fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
            return ERROR_F;
        }
        line_ptr++;
    }

    /* Extract rd destination register */
    line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
    if (token_reg[FIRST_INDEX] == '\0') {
        fprintf(stderr, "Error at file: %s, line %d: Too much ',' between operands, Or missing operands.\n", file_name, line_number);
        return ERROR_F;
    }
    if ((rd = register_num(token_reg, line_number, file_name)) == -1) return ERROR_F;

    /* Make sure no extra trailing characters exist at the end of the line */
    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of line. \"%s\"\n", file_name, line_number, line_ptr);
        return ERROR_F;
    }

    /* Build final 32-bit machine word */
    *coded_word = build_r_word(instr->opcode, rs, rt, rd, instr->funct);
    return SUCCESS_F;
}

/*
 * This Function analyzes operands for I-type assembly instructions and builds the binary word.
 *
 * Assumptions:
 * - instr points to a valid I-type instruction structure.
 * - line_ptr points to the remaining operand text of the line.
 *
 * Algorithm:
 * Extracts registers, immediate value, or label depending on the instruction type,
 * validates commas and operands format, ensures no extra trailing 
 * and builts the 32-bit mechine instruction word.
 */
int process_i_instruction(const Instruction *instr, char *line_ptr, long *coded_word, int line_number, char *file_name) {
    char token_reg[MAX_LINE_LENGTH];
    int rs = START_VALUE, rt = START_VALUE, immed = START_VALUE;

    if(is_comment(line_ptr) || is_empty_line(line_ptr)){
        fprintf(stderr, "Error at file: %s, line %d: Missing operands for instruction.\n", file_name, line_number);
        return ERROR_F;
    }
    /* Extract rs register */
    line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
        if (token_reg[FIRST_INDEX] == '\0') {
            fprintf(stderr, "Error at file: %s, line %d: Unexpected ',' before operands.\n", file_name, line_number);
            return ERROR_F;
        }
    if ((rs = register_num(token_reg, line_number, file_name)) == -1) return ERROR_F;

    /* Skip whitespace and check comma after rs */
    while (isspace(*line_ptr)) line_ptr++;
    if (*line_ptr != ',') {
        fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
        return ERROR_F;
    }
    line_ptr++;

    if (instr->type == TYPE_I_ALU || instr->type == TYPE_I_LOAD_STORE) {
        /* Extract immediate value for ALU / LOAD / STORE instructions */
        line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
        if (token_reg[FIRST_INDEX] == '\0') {
            fprintf(stderr, "Error at file: %s, line %d: Too much ',' between operands, Or missing operands.\n", file_name, line_number);
            return ERROR_F;
        }
        if ((immed = immediate_to_num(token_reg, line_number, file_name)) == WRONG_IMMED) return ERROR_F;

        /* Skip whitespace and check comma after immediate value */
        while (isspace(*line_ptr)) line_ptr++;
        if (*line_ptr != ',') {
            fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
            return ERROR_F;
        }
        line_ptr++;        
    }

    /* Extract rt register */
    line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
    if (token_reg[FIRST_INDEX] == '\0') {
        fprintf(stderr, "Error at file: %s, line %d: Too much ',' between operands, Or missing operands.\n", file_name, line_number);
        return ERROR_F;
    }
    if ((rt = register_num(token_reg, line_number, file_name)) == -1) return ERROR_F;

    if (instr->type == TYPE_I_BRANCH) {
        /* Skip whitespace and check comma after rt for branch instruction */
        while (isspace((unsigned char)*line_ptr)) line_ptr++;
        if (*line_ptr != ',') {
            fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
            return ERROR_F;
        }
        line_ptr++;
        /* Extract and validate target branch label */    
        line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER); /*care lable later*/
        if (token_reg[FIRST_INDEX] == '\0') {
            fprintf(stderr, "Error at file: %s, line %d: Too much ',' between operands, Or missing operands.\n", file_name, line_number);
            return ERROR_F;
        }
        if (!is_valide_name(token_reg , line_number, file_name, IS_NOT_MACRO)) return ERROR_F; /*בדיקה ששם התווית הוא שם הגיוני לתווית*/    
        if (!is_valide_name(token_reg , line_number, file_name, IS_NOT_MACRO)) return ERROR_F;     
    }

    /* Make sure no extra trailing characters exist at the end of the line */
    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line: \"%s\"\n", file_name, line_number, line_ptr);
        return ERROR_F;
    }

    /* Build final 32-bit machine word */
    *coded_word = build_i_word(instr->opcode, rs, rt, immed);
    return SUCCESS_F;
}

/*
 * This function analyzes operands for J-type assembly instructions and builds the binary word.
 *
 * Assumptions:
 * - instr points to a valid J-type instruction structure.
 * - line_ptr points to the remaining operand text of the line.
 *
 * Algorithm:
 * Extracts target operand (register or label) for non-HLT instructions, validates operand syntax, 
 * checks end-of-line content, and constructs the 32-bit mechine instruction word.
 */
int process_j_instruction(const Instruction *instr, char *line_ptr, long *coded_word, int line_number, char *file_name) {
    int reg = START_VALUE, address = START_VALUE;
    char token[MAX_LINE_LENGTH];

    /* Extract target operand (label or register) for non-HLT instructions */
    if (instr->type != TYPE_J_HLT) {
        line_ptr = extract_word(line_ptr, token, IS_REGISTER);
        /* Check for missing operand */
        if (token[FIRST_INDEX] == '\0') {
            fprintf(stderr, "Error at file: %s, line %d: Missing operand for instruction.\n", file_name, line_number);
            return ERROR_F;
        }
        
        if (instr->type == TYPE_J_JUMP) {
            /* analyze jump command to register (jmp $register) */
            if (token[FIRST_INDEX] == '$') {
                if ((address = (unsigned int)register_num(token, line_number, file_name)) == -1) return ERROR_F;
                reg = 1;
            }
        }
        else if (!is_valide_name(token, line_number, file_name, IS_NOT_MACRO)) return ERROR_F; /* Validate label name syntax */
    }

    /* Make sure no extra trailing characters exist at the end of the line */
    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line. \"%s\"\n", file_name, line_number, line_ptr);
        return ERROR_F;
    }

    /* Build final 32-bit machine word */
    *coded_word = build_j_word(instr->opcode, reg, address);
    return SUCCESS_F;
}

/*
 * This function stores a 32-bit instruction word into the code image memory array and updates the instruction counter.
 *
 * Assumptions:
 * - code_img is a valid pointer to a buffer for storing the binary code image.
 * - ic points to the current instruction counter value.
 *
 * Algorithm:
 * Validates available memory space, splits the 32-bit word into 4 bytes 
 * using Little Endian byte ordering, stores them in code_img, and Update ic.
 */
int add_to_code_image(unsigned char *code_img, int *ic, long coded_word, int line_number, char *file_name) {
    int index = *ic - IC_START_VALUE;
    int b;

    /* Check if there is enough space in the code image memory */
    if ((*ic + BYTES_PER_WORD) > MEMORY_SIZE) {
        fprintf(stderr, "Error at file: %s, line %d: No space at code inage\n", file_name, line_number);
        return MEMORY_ERROR;
    }

    /* Split 32-bit word into 4 bytes using little endian byte ordering */  
    for (b = START_VALUE; b < BYTES_PER_WORD; b++)
        code_img[index++] = (char)((coded_word >> (b * BITS_PER_BYTE)) & 0xFF);
    /* Increasing instruction counter by 4 bytes */
    *ic += BYTES_PER_WORD;
    
    return SUCCESS_F;
}

/*
 * This function gets an assembly instruction and send it to the appropriate function for analysis.
 *
 * Assumptions:
 * - word_instr contains a valid instruction string.
 * - code_img and ic point to valid code memory structures.
 *
 * Algorithm:
 * Search for the instruction definition, send it to processing to R, I, or J type function,
 * and writes the coded binary word to the code image.
 */
int process_instruction(char *word_instr, char *line_ptr, unsigned char *code_img, int *ic, int line_number, char *file_name) {
    const Instruction *instr = find_instruction(word_instr); /* Search the instruction word in the table */
    long coded_word = START_VALUE;
    
    /* Ensure  the instruction word exist */
    if (instr == NULL) {
        fprintf(stderr, "Error at file: %s, line %d: Unknown instruction '%s'.\n", file_name, line_number, word_instr);
        return ERROR_F;
    }
    
    /* Send to processing the instruction operands by its type */
    switch (instr->type) {
        case TYPE_R_ALU:
        case TYPE_R_MOVE:
        
            if (!process_r_instruction(instr, line_ptr, &coded_word, line_number, file_name)) return ERROR_F;
            break;
            
        case TYPE_I_ALU:
        case TYPE_I_BRANCH:
        case TYPE_I_LOAD_STORE:
        
            if (!process_i_instruction(instr, line_ptr, &coded_word, line_number, file_name)) return ERROR_F; 
            break;
        
        case TYPE_J_JUMP:
        case TYPE_J_LOAD_ADD:
        case TYPE_J_CALL:
        case TYPE_J_HLT:

            if (!process_j_instruction(instr, line_ptr, &coded_word, line_number, file_name)) return ERROR_F;
            break;
            
        default:
            return ERROR_F;
    }
    /* Store binary word into code image memory */
    if (add_to_code_image(code_img, ic, coded_word, line_number, file_name) == MEMORY_ERROR) return MEMORY_ERROR;
    
    return SUCCESS_F;
}

