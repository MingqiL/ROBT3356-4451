/*
 * UsciCmdInterpreter.c
 *
 *  Created on: Jan. 9, 2022
 *      Author: Andrew Friesen
 */

#include <CmdInterpreter.h>
#include <msp430.h>
#include <string.h>
#include <Uart_lab.h>
#include <stdlib.h>  // standard functions and constant
#include <stdio.h>   // i/o functions
#include <math.h>    // math functions
#include <ctype.h>   // character functions
#include <stdbool.h> // bool definitions
#include <nok5110LCD.h>

// list all commands
enum COMMAND_INDEX
{
    PIXEL, SCRNLINE, LINE, CLEAR, NUM_COMMANDS
};
CMD cmds[NUM_COMMANDS]={0};

const char *seps = "\t,\n ;:";       // for tokenizing the line string

/*
 * Name: cmdsinit
 *
 * Function:
 * Initializes the list of commands for the command interpreter
 *
 * Inputs: none
 * Returns: none
 */
void cmdsinit()
{
    //Load command list elements here
    cmds[PIXEL].name = "PIXEL";     // assign name wrt index
    cmds[PIXEL].nArgs = 2;          // assign command argument amount
    cmds[PIXEL].index = PIXEL;      // assign command index

    cmds[SCRNLINE].name = "SCRNLINE";
    cmds[SCRNLINE].nArgs = 3;
    cmds[SCRNLINE].index = SCRNLINE;

    cmds[LINE].name = "LINE";
    cmds[LINE].nArgs = 4;
    cmds[LINE].index = LINE;

    cmds[CLEAR].name = "CLEAR";
    cmds[CLEAR].nArgs = 0;
    cmds[CLEAR].index = CLEAR;


    //This must change whenever you add, modify, or remove commands
}

/*
 * Name: parseCmd
 *
 * Function:
 * Parses string for command name and arguments. Validates command by calling "validateCmd". If the command is valid
 * the function will parse the remaining args in the string into the of args array of the CMD struct. If the number of args
 * is not the same as nArgs for the command the function will return an error as the command is invalid.
 *
 * Input: [char *] String containing command name and comma delimited arguments.
 * Returns: [int] The command number. (It's element in the array of commands.) If the command is invalid return -1.
 */
int parseCmd(char * cmdLine)
{
    char * tok = NULL;
    int cmdNum = -1;
    unsigned char argIndex;

    //strcpy(cmdPiece,cmdLine);     // copy the command string to a string placeholder
    //Process command string into tokens for command name and any arguments for the command.
    tok = strtok(cmdLine, seps); //first token should give command

    //Use the "validateCmd" function to check the name token is a valid command name.
    cmdNum = validateCmd(tok);
    if(cmdNum == -1) ucsiA1UartTxString("Error! Can't find command!");

    // check nArg
    for (argIndex=0;argIndex<cmds[cmdNum].nArgs;argIndex++){
        tok = strtok(NULL,seps);
        if (tok == NULL) {  // too less argument
            ucsiA1UartTxString("ERROR!TOO LESS ARGUMENT!");
            return -1;
        }
        else
            cmds[cmdNum].args[argIndex] = atoi(tok);
    }
    // check for too many argument
    if (strtok(NULL,seps)!=NULL){
        ucsiA1UartTxString("ERROR! TOO MANY ARGUMENT!");
        return -1;
    }

    //This function should be written in a way that it can be used for ANY number of commands or arguments
    //and should not need to be modified if you use the command interpreter for other commands.

    return cmdNum;
}

/*
 * Name: validateCmd
 *
 * Function:
 * Called by "parseCmd" to compare the "command name" portion of the string being parsed against the list of commands.
 *
 * Input: [char *] The "command name" portion of a command string
 * Returns: [int] Returns the index of the command in the command list array for valid commands. Returns -1 for invalid commands.
 */
int validateCmd(char * cmdName)
{
    int cmdIndex;
    //Compare the name string token to the list of command names in your cmds array.
    makeStringUpperCase(cmdName);   // make all letters upper cases
    for(cmdIndex=0;cmdIndex<=NUM_COMMANDS;cmdIndex++){
        if(strcmp(cmdName,cmds[cmdIndex].name) == 0) return cmdIndex;      // if corresponding cmd is found, break the for loop
    }
    //This function should be written in a way that it can be used for ANY number of commands or arguments
    //and should not need to be modified if you use the command interpreter for other commands.
    return -1;
}

/*
 * Name: executeCMD
 *
 * Function:
 * Executes the selected command.
 *
 * Input: [int] The command number. (It's element in the array of commands.)
 * Return: [int] Return 0 if the command executes correctly. Return -1 for an invalid command.
 */
int executeCMD(int cmdIndex)
{
    unsigned char errMsg[BUFF_SZ] = "ERROR at executing CMD! INPUT OUR OF RANGE!";
    unsigned char x_start, x_end, y_start, y_end, line_mode;

    x_start = cmds[cmdIndex].args[0];
    y_start = cmds[cmdIndex].args[1];

    if (x_start>LCD_MAX_COL || y_start>LCD_MAX_ROW){
        ucsiA1UartTxString(errMsg);
        return -1;
    }

    //All code to execute the commands go in here.
    switch (cmdIndex){
    case PIXEL:     // (x,y)
        nokLcdSetPixel(x_start,y_start);
        break;

    case SCRNLINE:
        line_mode = cmds[cmdIndex].args[2];
        nokLcdDrawScrnLine(x_start,y_start,line_mode);
        break;

    case LINE:
        x_end = cmds[cmdIndex].args[2];
        y_end = cmds[cmdIndex].args[3];
        if (x_end>LCD_MAX_COL || y_end>LCD_MAX_ROW){
            ucsiA1UartTxString(errMsg);
            return -1;
        }
        nokLcdDrawLine(x_start,y_start,x_end,y_end);
        break;

    case CLEAR:
        nokLcdClear();
        break;


    default:
        ucsiA1UartTxString(errMsg);
        return -1;

    }

    return -1;
    //This must change whenever you add, modify, or remove commands
}


//---------------------------------------------------------------------------------------------------------------------
// DESCRIPTION:  makes a string all upper case characters
// ARGUMENTS:    str:  the string memory address
// RETURN VALUE: none
void makeStringUpperCase(char *str)
{
   unsigned char i;
   if(str == NULL) return; // safety!

   for(i = 0; i < strlen(str); i++) str[i] = (char)toupper(str[i]);
}
