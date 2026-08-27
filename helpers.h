#ifndef HELPERS_H
#define HELPERS_H


/*
* This file is a header containing declarations of general helpes functions for the assembler program.
* The functions are used to parse input lines ( comments, empty lines, labels),extract words and operands, creation output file names, and check the validity of label and macro names.
* The functions output success/error, new strings, and new file names as needed.
* Assumptions: All strings passed as parameters with a '\0'.
*/


/*
* Checks if the line is empty or with only spaces.
* Input:str is the string of line.
* Output: Returns SUCCESS_F-1 if is empty and ERROR_F-0 else.
*/
int is_empty_line( char *str);

/*
* Extracts only the word from a given line string.
* Input: str is the source string, result is  hold the word we want, is_operand is a flag if the word is an operand or not.
* Output: Returns a pointer to the location in the source string where the the result ends*/
char *extract_word(char *str, char *result,int is_operand);

/*
* Checks if word is a valid label definition-ending with ':'
* Input: word-is a string of the word.
* Output: Returns SUCCESS_F if it is a label and ERROR_F if not.
*/
int is_label(char *word);

/*
* Checks if the line is a comment line.
* Input: str-this the string of the line.
* Output: Returns SUCCESS_F if it is a comment and ERROR_F if not..
*/
int is_comment(char *str);

/*
* Creates a new file name with updated ending, according to the request.
* Input:name_file-this is the original file name string, ending-the new ending we want.
* Output:returns a pointer to the new file name string, or NULL in case of error.
*/
char *make_new_name_file(char *name_file, char *ending);

/*
* Checks if a label or macro name is valid according to the rules.
* Input:name-is the string name, line_number-is line number in the file,file_name-string file name of the file,is_mac-is flag if it is a macro or not.
* Output:Returns SUCCESS_F if the name is valid and ERROR_F if not and prints error.
*/
int is_valide_name(char *name,int line_number,char *file_name, int is_macro);


#endif