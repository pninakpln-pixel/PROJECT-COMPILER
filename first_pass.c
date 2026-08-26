#include "globals.h"
#include "helpers.h"
#include "first_pass.h" 

#define MAX_LINE_LEN 82
#define MAX_WORD_LEN 32
#define MEMORY_START 100
#define BYTES_PER_BYTE 1
#define BYTES_PER_WORD 4
#define BYTES_PER_HALF_WORD 2
#define BITS_PER_BYTE 8

enum status_fnc {ERROR, SUCCESS};
enum status_flg {OFF, ON};

#define WRONG_IMMED -32769

/*========================================================================================================================*/
/*Allocating memory space for data*/
int make_data_space(char **data_img, int dc, int bytes_to_add, int line_number, char *file_name) {
    char *temp = (char *)realloc(*data_img, dc + bytes_to_add);
    if (temp == NULL) {
        fprintf(stderr, "Error at file: %s, line %d: Memory allocation for data image failed!\n", file_name, line_number);
        return -1;
    }
    *data_img = temp;
    return 1;
}

int copy_num_data(char **data_img, int *dc, long *values, int count, int bytes_per_val, int line_number, char *file_name) {
    int total_bytes;
    int i, b;

    total_bytes = count * bytes_per_val;
    
     if (make_data_space(data_img, *dc, total_bytes, line_number, file_name) == -1) return -1;

    for (i = 0; i < count; i++) {
        long val = values[i];
        for (b = 0; b < bytes_per_val; b++) {
            (*data_img)[*dc] = (char)((val >> (b * 8)) & 0xFF);
            (*dc)++;
        }
    }

    return 1;
}

/* */
int extract_numbers_data(char *str, long *values_out, int *count_out, int bytes_per_val, int line_number, char *file_name) {
    char *ptr;
    char *endptr;
    int expecting_comma;
    long val;

    ptr = str;
    *count_out = 0;
    expecting_comma = 0;

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
                return 0;
            }

            val = strtol(ptr, &endptr, 10);

            if (ptr == endptr) {
                fprintf(stderr, "Error at file: %s, line %d: Invalid numeric token at '%s'.\n", file_name, line_number, ptr);
                return 0;
            }
            
            if (bytes_per_val == 1 && (val < -128 || val > 255)) {
                fprintf(stderr, "Error at file: %s, line %d: %ld number out of byte range.\n", file_name, line_number, val);
                return 0;
            } 
            if (bytes_per_val == 2 && (val < -32768 || val > 65535)) {
                fprintf(stderr, "Error at file: %s, line %d: %ld number out of half word range.\n", file_name, line_number, val);
                return 0;
            }
            if (bytes_per_val == 4 && (val < -2147483648 || val > 2147483647)) {
                fprintf(stderr, "Error at file: %s, line %d: %ld number out of word range.\n", file_name, line_number, val);
                return 0;
            }

            values_out[*count_out] = val;
            (*count_out)++;
            ptr = endptr; 
            expecting_comma = 1;   

        } 
        else {
            
            if (*ptr == ',') {
                expecting_comma = 0; 
                ptr++;
            } 
            else {
                fprintf(stderr, "Error at file: %s, line %d: Missing comma between operands before: \"%s\".\n", file_name, line_number, ptr);
                return 0;
            }
        }
    }

    if (expecting_comma == 0 && *count_out > 0) {
        fprintf(stderr, "Error at file: %s, line %d: Trailing comma at end of line.\n", file_name, line_number);
        return 0;
    }

    return 1;
}


int extract_copy_asciz_data (char *line_str, char **data_img, int *dc, int line_number, char *file_name) {
    int start_dc = *dc;

    while (isspace(*line_str)) 
        line_str++;

    if (*line_str != '"') {
        fprintf(stderr, "Error at file: %s, line %d: String must start with \".\n", file_name, line_number);
        return 0; 
    }
    line_str++; 
    
    if (make_data_space(data_img, *dc, MAX_LINE_LEN, line_number, file_name) == -1) return -1;

    while (*line_str != '"' && *line_str != '\0') {
        (*data_img)[(*dc)++] = *line_str++;
    }
    
    if (*line_str != '"') {
        fprintf(stderr, "Error at file: %s, line %d: String must end with \".\n", file_name, line_number);
        *dc = start_dc;
        return 0; 
    }
    line_str++;
    (*data_img)[(*dc)++] = '\0';

    /* אימות שלא נשארו תווים מיותרים בסוף השורה */
    if (!is_comment(line_str) && !is_empty_line(line_str)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line: \"%s\"\n", file_name, line_number, line_str);
        *dc = start_dc;
        return 0;
    }

    return 1;
}

/*========================================================================================================================*/

/**/
long build_r_word(int opcode, int rs, int rt, int rd, int funct) {
    long word = 0;

    word |= (opcode << 26);
    word |= (rs << 21);
    word |= (rt << 16);
    word |= (rd << 11);
    word |= (funct << 6);

    return word;
}

/**/
long build_i_word(int opcode, int rs, int rt, int immed) {
    long word = 0;
    
    word |= (opcode << 26);
    word |= (rs << 21);
    word |= (rt << 16);
    word |= (immed & 0xFFFF);
    
    return word;
}

/**/
long build_j_word(int opcode, int reg, int address) {
    long word = 0;

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
    char token_reg[MAX_LINE_LEN];
    int rs = 0, rt = 0, rd = 0;

     /* 1. חילוץ rs */
    line_ptr = extract_word(line_ptr, token_reg, 1);
    if ((rs = register_num(token_reg, line_number, file_name)) == -1) return 0;

    while (isspace(*line_ptr)) line_ptr++;
            
    if (*line_ptr != ',') {
        fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
        return 0;
    }
    line_ptr++;

    if (instr->type == TYPE_R_ALU) {

        /* 2. חילוץ rt */
        line_ptr = extract_word(line_ptr, token_reg, 1);
        if ((rt = register_num(token_reg, line_number, file_name)) == -1) return 0;

        while (isspace(*line_ptr)) line_ptr++;
            
        if (*line_ptr != ',') {
            fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
            return 0;
        }
        line_ptr++;
    }

    /* 2. חילוץ rd */
    line_ptr = extract_word(line_ptr, token_reg, 1);
    if ((rd = register_num(token_reg, line_number, file_name)) == -1) return 0;

    /* וידוא שלא נשארו תווים מיותרים בסוף השורה */
    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the ent of line. \"%s\"\n", file_name, line_number, line_ptr);
        return 0;
    }

    /* בניית המילה הבינארית */
    *coded_word = build_r_word(instr->opcode, rs, rt, rd, instr->funct);
    return 1;
}


int process_i_instruction(const Instruction *instr, char *line_ptr, long *coded_word, int line_number, char *file_name) {
    char token_reg[MAX_LINE_LEN];
    int rs = 0, rt = 0, immed = 0;

    /* 1. חילוץ rs */
    line_ptr = extract_word(line_ptr, token_reg, 1);
    if ((rs = register_num(token_reg, line_number, file_name)) == -1) return 0;

    /* בדיקת פסיק ראשון */
    while (isspace(*line_ptr)) line_ptr++;
    if (*line_ptr != ',') {
        fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
        return 0;
    }
    line_ptr++;

    if (instr->type == TYPE_I_ALU || instr->type == TYPE_I_LOAD_STORE) {

        /* 2. חילוץ immed (מספר מיידי) */
        line_ptr = extract_word(line_ptr, token_reg, 1);
        if ((immed = immediate_to_num(token_reg, line_number, file_name)) == WRONG_IMMED) return 0;

        /* בדיקת פסיק שני */
        while (isspace((unsigned char)*line_ptr)) line_ptr++;
        if (*line_ptr != ',') {
            fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
            return 0;
        }
        line_ptr++;        
    }

    /* 3. חילוץ rt */
    line_ptr = extract_word(line_ptr, token_reg, 1);
    if ((rt = register_num(token_reg, line_number, file_name)) == -1) return 0;

    if (instr->type == TYPE_I_BRANCH) {
           
        /* בדיקת פסיק שני */
        while (isspace((unsigned char)*line_ptr)) line_ptr++;
        if (*line_ptr != ',') {
            fprintf(stderr, "Error at file: %s, line %d: Expected ',' between operands.\n", file_name, line_number);
            return 0;
        }
        line_ptr++;
            
        line_ptr = extract_word(line_ptr, token_reg, 1); /*care lable later*/
    }

    /* אימות שלא נשארו תווים מיותרים בסוף השורה */
    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line: \"%s\"\n", file_name, line_number, line_ptr);
        return 0;
    }

    /* בניית המילה הבינארית */
    *coded_word = build_i_word(instr->opcode, rs, rt, immed);
    return 1;
}


int process_j_instruction(const Instruction *instr, char *line_ptr, long *coded_word, int line_number, char *file_name) {
    int reg = 0, address = 0;
    char token[MAX_LINE_LEN];

    /* חילוץ האופרנד היחיד (תווית או אוגר) */
    if (instr->type != TYPE_J_HLT)
        line_ptr = extract_word(line_ptr, token, 1);
  
    if (instr->type == TYPE_J_JUMP && token[0] != '\0') {
    
        if (token[0] == '$') {
            /* תחביר 2: jmp $register */
            if ((address = (unsigned int)register_num(token, line_number, file_name)) == -1) return 0;

            reg = 1;
        }

    }

    /* אימות שלא נשארו תווים מיותרים בסוף השורה */
    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line. \"%s\"\n", file_name, line_number, line_ptr);
        return 0;
    }

    /* בניית המילה הבינארית */
    *coded_word = build_j_word(instr->opcode, reg, address);
    return 1;
}


int add_to_code_image(unsigned char *code_img, int *ic, long coded_word, int line_number, char *file_name) {
    int index = *ic - MEMORY_START;
    int b;

    if ((*ic + 4) > MEMORY_SIZE) {
        fprintf(stderr, "Error at file: %s, line %d: No space at code inage\n", file_name, line_number);
        return -1;
    }

    /* פירוק המילה של ה-32 ביט ל-4 בתים (8 ביט) לפי Little Endian */  
    for (b = 0; b < BYTES_PER_WORD; b++)
        code_img[index++] = (char)((coded_word >> (b * BITS_PER_BYTE)) & 0xFF);
    /* קידום מונה ההוראות ב-4 בתים (מילה אחת) לקראת הפקודה הבאה */
    *ic += BYTES_PER_WORD;
    
    return 1; /* סיום בהצלחה */
}


int process_instruction(char *word_instr, char *line_ptr, unsigned char *code_img, int *ic, int line_number, char *file_name) {
    const Instruction *instr = find_instruction(word_instr);
    long coded_word = 0;
    
    if (instr == NULL) {
        fprintf(stderr, "Error at file: %s, line %d: Unknown instruction '%s'.\n", file_name, line_number, word_instr);
        return 0;
    }
    
    switch (instr->type) {
    
        case TYPE_R_ALU:
        case TYPE_R_MOVE:
        
            if (!process_r_instruction(instr, line_ptr, &coded_word, line_number, file_name)) return 0;
            break;
            
        case TYPE_I_ALU:
        case TYPE_I_BRANCH:
        case TYPE_I_LOAD_STORE:
        
            if (!process_i_instruction(instr, line_ptr, &coded_word, line_number, file_name)) return 0; 
            break;
        
        case TYPE_J_JUMP:
        case TYPE_J_LOAD_ADD:
        case TYPE_J_CALL:
        case TYPE_J_HLT:

            if (!process_j_instruction(instr, line_ptr, &coded_word, line_number, file_name)) return 0;
            break;
            
        default:
            return 0;
    }
    if (add_to_code_image(code_img, ic, coded_word, line_number, file_name) == -1) return -1;
    
    return 1;
}

/*========================================================================================================================*/


/* פונקציית המעבר הראשון הראשית*/
int first_pass(char *file_name, Symbol **symbol_head, unsigned char *code_img, char **data_img) {
    FILE *am_file;
    char line[MAX_LINE_LEN];
    int line_num = 0;
    int error_flag = 0;
    int ic = MEMORY_START;
    int dc = 0;
    
    am_file = fopen(file_name, "r");
    if (am_file == NULL) {
        fprintf(stderr, "Error: Opening file %s for first pass failed.\n", file_name);
        return 0;
    }

    while (fgets(line, MAX_LINE_LEN, am_file) != NULL) {
        char *line_ptr = line;
        char word[MAX_WORD_LEN];
        int exist_label = 0;
        char label_name[MAX_WORD_LEN];
        int process_instruction_status;

        line[strlen(line) - 1] = '\0';
        
        line_num++;
        
        /* 1. דילוג על רווחים והתעלמות משורות ריקות או הערות */
        if (is_comment(line_ptr) || is_empty_line(line_ptr)) {
            continue;
        }

        /* 2. בדיקה האם מוגדרת תווית בתחילת השורה */
        if (is_comment(line_ptr) || is_empty_line(line_ptr)) {
                fprintf(stderr, "Error at file: %s, line %d: Sould be eny instruction after lable\n", file_name, line_num);
                error_flag = 1;
                continue;
            }
        line_ptr = extract_word(line_ptr, word, 0);
        if (is_label(word)) {
            exist_label = ON;
            word[strlen(word) - 1] = '\0';  /* should delete ':' from the end of the lable name */
            strcpy(label_name ,word);
            line_ptr = extract_word(line_ptr, word, 0);

          
        }

        /* 3. בדיקה עבור הנחיות (.data, .asciz, .extern, .entry, .db, .dh, .dw) */
        if (word[0] == '.') {
            if (!strcmp(word, ".db") || !strcmp(word, ".dw") || !strcmp(word, ".dh") || !strcmp(word, ".asciz")) {
                if (exist_label) {
                    int add_symbol_status = add_symbol(symbol_head, label_name, dc, SYMBOL_DATA, line_num, file_name);
                    if (add_symbol_status == -1) return -1;
                    if (add_symbol_status == 0) error_flag = 1;
                }
                /* שלב 8: זהה את סוג הנתונים, קודד אותם לתמונת הזיכרון, והגדל את DC. 
                   (נניח שיש פונקציית עזר שעושה זאת לפי ההנחיה והפרמטרים). */
                if (strcmp(word, ".asciz") == 0) {
                    int extract_copy_asciz_data_state = extract_copy_asciz_data (line_ptr, data_img, &dc, line_num, file_name);
                    if (extract_copy_asciz_data_state == -1) return -1;
                    if (extract_copy_asciz_data_state == 0) error_flag = 1;
                } else {
                    long temp_arr[MAX_LINE_LEN] = {0};
                    int count = 0, bytes_per_val;
                    if (strcmp(word, ".db") == 0) bytes_per_val = BYTES_PER_BYTE;
                    else if (strcmp(word, ".dw") == 0) bytes_per_val = BYTES_PER_WORD;
                    else if (strcmp(word, ".dh") == 0) bytes_per_val = BYTES_PER_HALF_WORD;
                    if (!extract_numbers_data(line_ptr, temp_arr, &count, bytes_per_val, line_num, file_name)) error_flag = 1;
                    if (copy_num_data(data_img, &dc, temp_arr, count, bytes_per_val, line_num, file_name) == -1) return -1;
                }
                /* חזור ל-2 */
                continue; 

            } else if (!strcmp(word, ".entry") || !strcmp(word, ".extern")) {
                
                /* שלב 10: אם זו הנחית entry., חזור ל-2 (מטופל במעבר שני) */
                if (strcmp(word, ".entry") == 0) {
                    if (exist_label)
                        fprintf(stderr, "Warning for file: %s, line %d: A label defined at the beginning of a '.entry' line is meaningless", file_name, line_num);
                    line_ptr = extract_word(line_ptr, word, 0);/*care lable operand later*/

                    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
                        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line: \"%s\"\n", file_name, line_num, line_ptr);
                        error_flag = 1;
                    }                    
                    continue;
                }
                
                /* שלב 11: אם זו הנחית extern., הכנס את הסמל לטבלה עם ערך 0 ומאפיין external */
                if (strcmp(word, ".extern") == 0) {
                    int add_symbol_status;
                    
                    if (exist_label)
                      fprintf(stderr, "Warning for file: %s, line %d: A label defined at the beginning of a '.extern' line is meaningless", file_name, line_num);
                
                    line_ptr = extract_word(line_ptr, label_name, 0);
                    
                    add_symbol_status = add_symbol(symbol_head, label_name, 0, SYMBOL_EXTERN, line_num, file_name);
                    if (add_symbol_status == -1) return -1;
                    if (add_symbol_status == 0) error_flag = 1;

                    if (!is_comment(line_ptr) && !is_empty_line(line_ptr)) {
                        fprintf(stderr, "Error at file: %s, line %d: Invalid characters at the end of the line: \"%s\"\n", file_name, line_num, line_ptr);
                        error_flag = 1;
                    }
                    continue;
                }
            }
            else {
                fprintf(stderr, "Error at file: %s, line %d: Unknown instruction '%s'\n", file_name, line_num, word);
                error_flag = 1;
                continue;
            } 
        }  
        /* שלב 12: זוהי שורת הוראה. אם יש תווית, הכנס לטבלה עם המאפיין code וערך IC */
        if (exist_label) {
            int add_symbol_status = add_symbol(symbol_head, label_name, ic, SYMBOL_CODE, line_num, file_name);
            if (add_symbol_status == -1) return -1;
            if (add_symbol_status == 0) error_flag = 1;    
        }
        /* שלבים 13, 14, 15: חפש פקודה, נתח אופרנדים, קודד והוסף לתמונת הקוד. 
           העברנו את האחריות הזו לפונקציה מרוכזת (שתדפיס שגיאות במידת הצורך). */
  
        process_instruction_status = process_instruction(word, line_ptr, code_img, &ic, line_num, file_name);
        if (process_instruction_status == -1) return -1;
        if (process_instruction_status == 0) error_flag = 1;
    }           
   /* סוף לולאת הקריאה (שלב 2) */
    fclose(am_file);

    /* שלב 17: אם נמצאו שגיאות במעבר הראשון, עצור כאן. */
    if (error_flag) return 0;

    /* שלב 18: שמור את הערכים הסופיים של IC ושל DC (שנקראים ICF ו-DCF) */
    ICF = ic;
    DCF = dc;
    /* שלב 19: עדכן בטבלת הסמלים את ערכו של כל סמל המאופיין כ-data, ע"י הוספת הערך ICF */
    update_data_symbols_address(*symbol_head, ic);


    /* שלב 21: התחל מעבר שני */
    return 1; /* מחזירים 1 שמסמל "הצלחה", והתוכנית הראשית תקרא ל-second_pass */
}

