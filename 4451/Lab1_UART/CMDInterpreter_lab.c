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

// list all commands
enum COMMAND_INDEX
{
    RELAY, ENABLE, ABSPOSITION, NUM_COMMANDS
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
    cmds[RELAY].name = "RELAY";     // assign name wrt index
    cmds[RELAY].nArgs = 1;          // assign command argument amount
    cmds[RELAY].index = RELAY;      // assign command index

    cmds[ENABLE].name = "ENABLE";
    cmds[ENABLE].nArgs = 1;
    cmds[ENABLE].index = ENABLE;

    cmds[ABSPOSITION].name = "ABSPOSITION";
    cmds[ABSPOSITION].nArgs = 1;
    cmds[ABSPOSITION].index = ABSPOSITION;


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
    unsigned char argIndex = 0;
    unsigned char errMsg[BUFF_SZ] = "ERROR at executing CMD! INPUT OUR OF RANGE!";

    //All code to execute the commands go in here.
    switch (cmdIndex){
    case RELAY:
        for (argIndex=0;argIndex<cmds[RELAY].nArgs;argIndex++){
            switch(cmds[RELAY].args[argIndex]){
            case 0: //Sets relay control to PLC
                ucsiA1UartTxString("RELAY 0");
                P1OUT &= ~BIT5;
                break;
            case 1: //Sets relay control to microcontroller
                ucsiA1UartTxString("RELAY 1");
                P1OUT |= BIT5;
                break;
            default:    // error case
                ucsiA1UartTxString(errMsg);
                break;
            }
        }
        break;
    case ENABLE:
        for (argIndex=0;argIndex<cmds[ENABLE].nArgs;argIndex++){
            switch(cmds[ENABLE].args[argIndex]){
            case 0: // Disable motor
                ucsiA1UartTxString("ENABLE 0");
                P1OUT |= BIT2;
                break;
            case 1: // Enables the motor
                ucsiA1UartTxString("ENABLE 1");
                P1OUT &= ~BIT2;
                break;
            default:    // error case
                ucsiA1UartTxString(errMsg);
                break;
            }
        }
        break;
    case ABSPOSITION:
        for (argIndex=0;argIndex<cmds[ABSPOSITION].nArgs;argIndex++){
            switch(cmds[ABSPOSITION].args[argIndex]){
            case 0: // Move to position 0
                ucsiA1UartTxString("ABSPOSITION 0");
                P1OUT |= BIT3 + BIT4;
                break;
            case 1: // Move to position 1
                ucsiA1UartTxString("ABSPOSITION 1");
                P1OUT |= BIT4;
                P1OUT &= ~BIT3;
                break;
            case 2: // Move to position 2
                ucsiA1UartTxString("ABSPOSITION 2");
                P1OUT |= BIT3;
                P1OUT &= ~BIT4;
                break;
            case 3: // Move to position 3
                ucsiA1UartTxString("ABSPOSITION 3");
                P1OUT &= ~(BIT3 + BIT4);
                break;
            default:    // error case
                ucsiA1UartTxString(errMsg);
                break;
            }
        }
        break;
    default:
        ucsiA1UartTxString("Error!");
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
