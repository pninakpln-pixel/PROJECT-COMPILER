#include "globals.h"
#include "helpers.h"
#include "first_pass.h" 

/*...............................
#define MAX_NAME_LENGTH 32

#define MAX_WORD_LEN 32
#define BYTES_PER_BYTE 1
#define BYTES_PER_WORD 4
#define BYTES_PER_HALF_WORD 2
#define BITS_PER_BYTE 8

#define FIRST_INDEX 0


#define IS_REGISTER 1
#define IS_NOT_REGISTER 0

#define IS_MACRO 1
#define IS_NOT_MACRO 0

#define WRONG_IMMED -32769
...............................*/

/*========================================================================================================================*/
/*Allocating memory space for data*/
int make_data_space(char **data_img, int dc, int bytes_to_add, int line_number, char *file_name) {
    char *temp = (char *)realloc(*data_img, dc + bytes_to_add);
    if (temp == NULL) {
        fprintf(stderr, "Error at file: %s, line %d: Memory allocation for data image failed!\n", file_name, line_number);
        return MEMORY_ERROR;
    }
    *data_img = temp;
    return SUCCESS_F;
}

int copy_num_data(char **data_img, int *dc, long *values, int count, int bytes_per_val, int line_number, char *file_name) {
    int total_bytes;
    int i, b;

    total_bytes = count * bytes_per_val;
    
     if (make_data_space(data_img, *dc, total_bytes, line_number, file_name) == MEMORY_ERROR) return MEMORY_ERROR;

    for (i = START_VALUE; i < count; i++) {
        long val = values[i];
        for (b = START_VALUE; b < bytes_per_val; b++) {
            (*data_img)[*dc] = (char)((val >> (b * BITS_PER_BYTE)) & 0xFF);
            (*dc)++;
        }
    }

    return SUCCESS_F;
}

/* */
int extract_numbers_data(char *str, long *values_out, int *count_out, int bytes_per_val, int line_number, char *file_name) {
    char *ptr;
    char *endptr;
    int expecting_comma;
    long val;

    ptr = str;
    *count_out = 0;
    expecting_comma = OFF;

    while (*ptr != '\0') {
        while (isspace(*ptr)) {
            ptr++;
        }

        if (*ptr == '\0') {
            break;
        }

        if (!expecting_comma) {
            if (*ptr == ',') {
                fprintf(stderr, "Error at file: %s, line %d: Unexpected comma found.\n", file_name, line_number);
                return ERROR_F;
            }

            val = strtol(ptr, &endptr, 10);

            if (ptr == endptr) {
                fprintf(stderr, "Error at file: %s, line %d: Invalid numeric token at '%s'.\n", file_name, line_number, ptr);
                return ERROR_F;
            }
            
            if (bytes_per_val == BYTES_PER_BYTE && (val < -128 || val > 255)) {
                fprintf(stderr, "Error at file: %s, line %d: %ld number out of byte range.\n", file_name, line_number, val);
                return ERROR_F;
            } 
            if (bytes_per_val == BYTES_PER_HALF_WORD && (val < -32768 || val > 65535)) {
                fprintf(stderr, "Error at file: %s, line %d: %ld number out of half word range.\n", file_name, line_number, val);
                return ERROR_F;
            }
            if (bytes_per_val == BYTES_PER_WORD && (val < (-2147483647 - 1) || val > 2147483647)) {
                fprintf(stderr, "Error at file: %s, line %d: %ld number out of word range.\n", file_name, line_number, val);
                return ERROR_F;
            }

            values_out[*count_out] = val;
            (*count_out)++;
            ptr = endptr; 
            expecting_comma = ON;   

        } 
        else {
            
            if (*ptr == ',') {
                expecting_comma = OFF; 
                ptr++;
            } 
            else {
                fprintf(stderr, "Error at file: %s, line %d: Missing comma between operands before: \"%s\".\n", file_name, line_number, ptr);
                return ERROR_F;
            }
        }
    }

    if (expecting_comma == OFF && *count_out > 0) {
        fprintf(stderr, "Error at file: %s, line %d: Trailing comma at end of line.\n", file_name, line_number);
        return ERROR_F;
    }

    return SUCCESS_F;
}


int extract_copy_asciz_data (char *line_str, char **data_img, int *dc, int line_number, char *file_name) {
    int start_dc = *dc;

    while (isspace(*line_str)) 
        line_str++;

    if (*line_str != '"') {
        fprintf(stderr, "Error at file: %s, line %d: String must start with \".\n", file_name, line_number);
        return ERROR_F; 
    }
    line_str++; 
    
    if (make_data_space(data_img, *dc, MAX_LINE_LENGTH, line_number, file_name) == MEMORY_ERROR) return MEMORY_ERROR;

    while (*line_str != '"' && *line_str != '\0') {
        (*data_img)[(*dc)++] = *line_str++;
    }
    
    if (*line_str != '"') {
        fprintf(stderr, "Error at file: %s, line %d: String must end with \".\n", file_name, line_number);
        *dc = start_dc;
        return ERROR_F; 
    }
    line_str++;
    (*data_img)[(*dc)++] = '\0';

    /* אימות שלא נשארו תווים מיותרים בסוף השורה */
    if (!is_comment(line_str) && !is_empty_line(line_str)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line: \"%s\"\n", file_name, line_number, line_str);
        *dc = start_dc;
        return ERROR_F;
    }

    return SUCCESS_F;
}

/*========================================================================================================================*/

/**/
long build_r_word(int opcode, int rs, int rt, int rd, int funct) {
    long word = START_VALUE;

    word |= (opcode << 26);
    word |= (rs << 21);
    word |= (rt << 16);
    word |= (rd << 11);
    word |= (funct << 6);

    return word;
}

/**/
long build_i_word(int opcode, int rs, int rt, int immed) {
    long word = START_VALUE;
    
    word |= (opcode << 26);
    word |= (rs << 21);
    word |= (rt << 16);
    word |= (immed & 0xFFFF);
    
    return word;
}

/**/
long build_j_word(int opcode, int reg, int address) {
    long word = START_VALUE;

    word |= (opcode << 26);
    word |= (reg << 25);
    word |= (address & 0x1FFFFFF);
    
    return word;
}

/**/
int register_num (char *str, int line_number, char *file_name) {
    char *endptr;
    int reg_num;

    if (*str != '$') {
        fprintf(stderr, "Error at file: %s, line %d: Expected '$' for register: '%s'.\n", file_name, line_number, str);
        return -1;
    }
    str++; 
    
    reg_num = (int)strtol(str, &endptr, 10);
    
    if (str == endptr || *endptr != '\0') {
        fprintf(stderr, "Error at file: %s, line %d: Invalid register number '%s'.\n", file_name, line_number, str);
        return -1;
    }
    
    if (reg_num < 0 || reg_num > 31) {
        fprintf(stderr, "Error at file: %s, line %d: Register number $%d out of range (0-31).\n", file_name, line_number, reg_num);
        return -1;
    }
    
    return reg_num;
}

/**/
int immediate_to_num(char *str, int line_number, char *file_name) {
    char *endptr;
    int val;

    val = (int)strtol(str, &endptr, 10);

    /* וידוא שהומרו ספרות בלבד ללא אותיות או תווי זבל */
    if (str == endptr || *endptr != '\0') {
        fprintf(stderr, "Error at file: %s, line %d: Invalid immediate number '%s'.\n", file_name, line_number, str);
        return WRONG_IMMED; /* המחרוזת אינה מספר טהור (למשל תווית) */
    }

    /* בדיקת טווח 16 ביט בשיטת המשלים ל-2 */
    if (val < -32768 || val > 32767) {
        fprintf(stderr, "Error at file: %s, line %d: Immediate value %d out of 16-bit range.\n", file_name, line_number, val);
        return WRONG_IMMED;
    }

    return val;
}

/**/
int process_r_instruction(const Instruction *instr, char *line_ptr, long *coded_word, int line_number, char *file_name) {
    char token_reg[MAX_LINE_LENGTH];
    int rs = START_VALUE, rt = START_VALUE, rd = START_VALUE;

     /* 1. חילוץ rs 0*/
    line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
    if ((rs = register_num(token_reg, line_number, file_name)) == -1) return ERROR_F;

    while (isspace(*line_ptr)) line_ptr++;
            
    if (*line_ptr != ',') {
        fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
        return ERROR_F;
    }
    line_ptr++;

    if (instr->type == TYPE_R_ALU) {

        /* 2. חילוץ rt */
        line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
        if ((rt = register_num(token_reg, line_number, file_name)) == -1) return ERROR_F;

        while (isspace(*line_ptr)) line_ptr++;
            
        if (*line_ptr != ',') {
            fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
            return ERROR_F;
        }
        line_ptr++;
    }

    /* 2. חילוץ rd */
    line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
    if ((rd = register_num(token_reg, line_number, file_name)) == -1) return ERROR_F;

    /* וידוא שלא נשארו תווים מיותרים בסוף השורה */
    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of line. \"%s\"\n", file_name, line_number, line_ptr);
        return ERROR_F;
    }

    /* בניית המילה הבינארית */
    *coded_word = build_r_word(instr->opcode, rs, rt, rd, instr->funct);
    return SUCCESS_F;
}


int process_i_instruction(const Instruction *instr, char *line_ptr, long *coded_word, int line_number, char *file_name) {
    char token_reg[MAX_LINE_LENGTH];
    int rs = START_VALUE, rt = START_VALUE, immed = START_VALUE;

    /* 1. חילוץ rs */
    line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
    if ((rs = register_num(token_reg, line_number, file_name)) == -1) return ERROR_F;

    /* בדיקת פסיק ראשון 0*/
    while (isspace(*line_ptr)) line_ptr++;
    if (*line_ptr != ',') {
        fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
        return ERROR_F;
    }
    line_ptr++;

    if (instr->type == TYPE_I_ALU || instr->type == TYPE_I_LOAD_STORE) {

        /* 2. חילוץ immed (מספר מיידי) */
        line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
        if ((immed = immediate_to_num(token_reg, line_number, file_name)) == WRONG_IMMED) return ERROR_F;

        /* בדיקת פסיק שני */
        while (isspace((unsigned char)*line_ptr)) line_ptr++;
        if (*line_ptr != ',') {
            fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
            return ERROR_F;
        }
        line_ptr++;        
    }

    /* 3. חילוץ rt */
    line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER);
    if ((rt = register_num(token_reg, line_number, file_name)) == -1) return ERROR_F;

    if (instr->type == TYPE_I_BRANCH) {
           
        /* בדיקת פסיק שני */
        while (isspace((unsigned char)*line_ptr)) line_ptr++;
        if (*line_ptr != ',') {
            fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
            return ERROR_F;
        }
        line_ptr++;
            
        line_ptr = extract_word(line_ptr, token_reg, IS_REGISTER); /*care lable later*/
    }

    /* אימות שלא נשארו תווים מיותרים בסוף השורה */
    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line: \"%s\"\n", file_name, line_number, line_ptr);
        return ERROR_F;
    }

    /* בניית המילה הבינארית */
    *coded_word = build_i_word(instr->opcode, rs, rt, immed);
    return SUCCESS_F;
}


int process_j_instruction(const Instruction *instr, char *line_ptr, long *coded_word, int line_number, char *file_name) {
    int reg = START_VALUE, address = START_VALUE;
    char token[MAX_LINE_LENGTH];

    /* חילוץ האופרנד היחיד (תווית או אוגר) */
    if (instr->type != TYPE_J_HLT)
        line_ptr = extract_word(line_ptr, token, IS_REGISTER);
  
    if (instr->type == TYPE_J_JUMP && token[FIRST_INDEX] != '\0') {
    
        if (token[FIRST_INDEX] == '$') {
            /* תחביר 2: jmp $register */
            if ((address = (unsigned int)register_num(token, line_number, file_name)) == -1) return ERROR_F;

            reg = 1;
        }

    }

    /* אימות שלא נשארו תווים מיותרים בסוף השורה */
    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line. \"%s\"\n", file_name, line_number, line_ptr);
        return ERROR_F;
    }

    /* בניית המילה הבינארית */
    *coded_word = build_j_word(instr->opcode, reg, address);
    return SUCCESS_F;
}


int add_to_code_image(unsigned char *code_img, int *ic, long coded_word, int line_number, char *file_name) {
    int index = *ic - IC_START_VALUE;
    int b;

    if ((*ic + BYTES_PER_WORD) > MEMORY_SIZE) {
        fprintf(stderr, "Error at file: %s, line %d: No space at code inage\n", file_name, line_number);
        return MEMORY_ERROR;
    }

    /* פירוק המילה של ה-32 ביט ל-4 בתים (8 ביט) לפי Little Endian */  
    for (b = START_VALUE; b < BYTES_PER_WORD; b++)
        code_img[index++] = (char)((coded_word >> (b * BITS_PER_BYTE)) & 0xFF);
    /* קידום מונה ההוראות ב-4 בתים (מילה אחת) לקראת הפקודה הבאה */
    *ic += BYTES_PER_WORD;
    
    return SUCCESS_F; /* סיום בהצלחה */
}


int process_instruction(char *word_instr, char *line_ptr, unsigned char *code_img, int *ic, int line_number, char *file_name) {
    const Instruction *instr = find_instruction(word_instr);
    long coded_word = START_VALUE;
    
    if (instr == NULL) {
        fprintf(stderr, "Error at file: %s, line %d: Unknown instruction '%s'.\n", file_name, line_number, word_instr);
        return ERROR_F;
    }
    
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
    if (add_to_code_image(code_img, ic, coded_word, line_number, file_name) == MEMORY_ERROR) return MEMORY_ERROR;
    
    return SUCCESS_F;
}

/*==========================0==============================================================================================*/


/* פונקציית המעבר הראשון הראשית*/
int first_pass(char *file_name, Symbol **symbol_head, unsigned char *code_img, char **data_img) {
    FILE *am_file;
    char line[MAX_LINE_LENGTH];
    int line_num = START_VALUE;
    int error_flag = OFF;
    int ic = IC_START_VALUE;
    int dc = DC_START_VALUE;
    
    am_file = fopen(file_name, "r");
    if (am_file == NULL) {
        fprintf(stderr, "Error: Opening file %s for first pass failed.\n", file_name);
        return ERROR_F;
    }

    while (fgets(line, MAX_LINE_LENGTH, am_file) != NULL) {
        char *line_ptr = line;
        char word[MAX_WORD_LEN];
        int exist_label = OFF;
        char label_name[MAX_WORD_LEN];
        int process_instruction_status;

        line[strlen(line) - 1] = '\0';
        
        line_num++;
        
        /* 1. דילוג על רווחים והתעלמות משורות ריקות או הערות */
        if (is_comment(line_ptr) || is_empty_line(line_ptr)) {
            continue;
        }

        /* 2. בדיקה האם מוגדרת תווית בתחילת השורה */
        line_ptr = extract_word(line_ptr, word, IS_NOT_REGISTER);
        if (is_label(word)) {
            exist_label = ON;
            word[strlen(word) - 1] = '\0';  /* should delete ':' from the end of the lable name */
            strcpy(label_name ,word);

            if (is_comment(line_ptr) || is_empty_line(line_ptr)) {
                fprintf(stderr, "Error at file: %s, line %d: Sould be eny instruction after lable\n", file_name, line_num);
                error_flag = ON;
                continue;
            }

            line_ptr = extract_word(line_ptr, word, IS_NOT_REGISTER);
        }

        /* 3. בדיקה עבור הנחיות (.data, .asciz, .extern, .entry, .db, .dh, .dw) */
        if (word[FIRST_INDEX] == '.') {
            if (!strcmp(word, ".db") || !strcmp(word, ".dw") || !strcmp(word, ".dh") || !strcmp(word, ".asciz")) {
                if (exist_label) {
                    int add_symbol_status = add_symbol(symbol_head, label_name, dc, SYMBOL_DATA, line_num, file_name);
                    if (add_symbol_status == MEMORY_ERROR) return MEMORY_ERROR;
                    if (!add_symbol_status) error_flag = ON;
                }
                /* שלב 8: זהה את סוג הנתונים, קודד אותם לתמונת הזיכרון, והגדל את DC. 
                   (נניח שיש פונקציית עזר שעושה זאת לפי ההנחיה והפרמטרים). */
                if (strcmp(word, ".asciz") == 0) {
                    int extract_copy_asciz_data_state = extract_copy_asciz_data (line_ptr, data_img, &dc, line_num, file_name);
                    if (extract_copy_asciz_data_state == MEMORY_ERROR) return MEMORY_ERROR;
                    if (!extract_copy_asciz_data_state) error_flag = ON;
                } else {
                    long temp_arr[MAX_LINE_LENGTH] = {START_VALUE};
                    int count = START_VALUE, bytes_per_val;
                    if (strcmp(word, ".db") == 0) bytes_per_val = BYTES_PER_BYTE;
                    else if (strcmp(word, ".dw") == 0) bytes_per_val = BYTES_PER_WORD;
                    else if (strcmp(word, ".dh") == 0) bytes_per_val = BYTES_PER_HALF_WORD;
                    if (!extract_numbers_data(line_ptr, temp_arr, &count, bytes_per_val, line_num, file_name)) error_flag = ON;
                    if (copy_num_data(data_img, &dc, temp_arr, count, bytes_per_val, line_num, file_name) == MEMORY_ERROR) return MEMORY_ERROR;
                }
                /* חזור ל-2 */
                continue; 

            } else if (!strcmp(word, ".entry") || !strcmp(word, ".extern")) {
                
                /* שלב 10: אם זו הנחית entry., חזור ל-2 (מטופל במעבר שני) */
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
                
                /* שלב 11: אם זו הנחית extern., הכנס את הסמל לטבלה עם ערך 0 ומאפיין external */
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
                fprintf(stderr, "Error at file: %s, line %d: Unknown instruction '%s'\n", file_name, line_num, word);
                error_flag = ON;
                continue;
            } 
        }  
        /* שלב 12: זוהי שורת הוראה. אם יש תווית, הכנס לטבלה עם המאפיין code וערך IC */
        if (exist_label) {
            int add_symbol_status = add_symbol(symbol_head, label_name, ic, SYMBOL_CODE, line_num, file_name);
            if (add_symbol_status == MEMORY_ERROR) return MEMORY_ERROR;
            if (!add_symbol_status) error_flag = ON;    
        }
        /* שלבים 13, 14, 15: חפש פקודה, נתח אופרנדים, קודד והוסף לתמונת הקוד. 
           העברנו את האחריות הזו לפונקציה מרוכזת (שתדפיס שגיאות במידת הצורך). */
  
        process_instruction_status = process_instruction(word, line_ptr, code_img, &ic, line_num, file_name);
        if (process_instruction_status == MEMORY_ERROR) return MEMORY_ERROR;
        if (!process_instruction_status) error_flag = ON;
    }           
   /* סוף לולאת הקריאה (שלב 2) */
    fclose(am_file);

    /* שלב 17: אם נמצאו שגיאות במעבר הראשון, עצור כאן. */
    if (error_flag) return ERROR_F;

    /* שלב 18: שמור את הערכים הסופיים של IC ושל DC (שנקראים ICF ו-DCF) */
    ICF = ic;
    DCF = dc;
    /* שלב 19: עדכן בטבלת הסמלים את ערכו של כל סמל המאופיין כ-data, ע"י הוספת הערך ICF */
    update_data_symbols_address(*symbol_head, ic);


    /* שלב 21: התחל מעבר שני */
    return SUCCESS_F; /* מחזירים 1 שמסמל "הצלחה", והתוכנית הראשית תקרא ל-second_pass */
}
