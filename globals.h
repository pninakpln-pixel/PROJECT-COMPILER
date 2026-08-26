#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MEMORY_SIZE 4096/* Maximum array size in computer memory */
#define MAX_LINE_LENGTH 82 /* Maximum line length, 80 characters + \n + \0 */
#define MAX_LABEL_LENGTH 32 /* Maximum label length, 31 characters + \0 */
#define MAX_M_L_CHAR 31 /* Maximum  31 characters in macro and label*/
#define MAX_MACRO_LENGTH 32/* Maximum length for name, 31 characters + '\0' */
#define MAX_WORD_LENGTH 32/* Maximum length for any word */
#define MEMORY_ERROR -1/*Returning an error in case of lack of memory space*/
#define IC_START_VALUE 100/* The starting address of the instruction image */
#define DC_START_VALUE 0/* The starting address of the data image */
#define START_VALUE 0/*Initialize to value 0*/
#define END_OF_STRING_CHAR 1/* Size of space for end of string character \0 */
#define TO_LAST_CHAR 1/* Subtract 1 to get to the last character */
#define WRONG_IMMED -32769
#define NO_CHAR 0/* no chare in string*/
#define ON 1/*Flag on*/
#define OFF 0 /*Flag off*/
#define SUCCESS_F 1/* Indicates success of a function*/
#define ERROR_F 0/*Indicates a function failure.*/
#define ERROR_MAIN 1/*An error was detected in main*/
#define SUCCESS_MAIN 0/*No errors and success returned for main*/
#define SAME 0/*result os compare function*/

#define NO_FILE_ARG 1/* Number of arguments that are not input files */
#define MIN_ARG 2 /* Minimum number of arguments*/
#define FIRST_INDEX 0/*First index of the array*/
#define ARRAY_UPDATE 0 /*Updating the array to 0 for the next file*/
#define MIN_FILE_NAME_LENGTH 4/*Minimum valid length for an input file name*/
#define LEN_AS 3/*The length of the file extension string (.as)*/
#define ERROR_EXIT 1/*Exiting the program in case of a memory error*/
#define SECOND_BYTE 1/*The second byte in a 4-byte instruction word */
#define THIRD_BYTE 2/*The third byte in a 4-byte instruction word */
#define FOURTH_BYTE 3/*The fourth byte in a 4-byte instruction word */

#define IS_REGISTER 1/* The operand is a register */
#define IS_NOT_REGISTER 0/* The operand is not a register */
#define IS_MACRO 1/* The operand is a macro */
#define IS_NOT_MACRO 0/* The operand is not a macro */
#define IS_NOT_ENTRY 0/* The operand is not entry*/
#define IS_ENTRY 1/* The operand is entry*/
#define BYTES_PER_BYTE 1
#define BYTES_PER_WORD 4/* Number of bytes in a full word*/
#define BYTES_PER_HALF_WORD 2/* Number of bytes in a half word*/
#define BITS_PER_BYTE 8
#define EXTERN_ADDRESS 0/*Address value for extern label */

#define MASK_OF_BYTE 0xFF/* 8-bit mask (single byte) */
#define MASK_for_SINGLE_BIT 0x01/* 1-bit mask */
#define ONE_BYTE 8/*Shift by one byte-8 bits */
#define TWO_BYTES 16/*Shift by two bytes-16 bits */
#define THREE_BYTES 24/*Shift by three bytes-24 bits */
#define NUM_INSTRUCTIONS 27
extern int ICF;/*Final instruction counter value*/
extern int DCF;/*Final data counter value*/


#endif