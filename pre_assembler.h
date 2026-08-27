#ifndef PRE_MACRO_ASSEMBLER_H
#define PRE_MACRO_ASSEMBLER_H

#include "globals.h"

/*
*The function performs the Pre-Assembler step.
*The function reads the source file, identifies and open all macros, and produces an extended source file (.am) ready for further steps.
*Input: name_file, a pointer to a string name of the source file.
*Output: Returns SUCCESS_F if the process passed successfully-the .am file was created without errors.
* Returns ERROR_F if errors were detected.
*/
int pre_assembler(char *name_file);

#endif
