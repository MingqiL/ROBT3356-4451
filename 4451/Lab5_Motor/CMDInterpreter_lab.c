/*
 * UsciCmdInterpreter.c
 *
 *  Created on: Jan. 9, 2022
 *      Author: Andrew Friesen
 */
#include <CmdInterpreter.h>
#include <msp430.h>
#include <string.h>
#include <stdlib.h>  // standard functions and constant
#include <stdio.h>   // i/o functions
#include <stdint.h>
#include <math.h>    // math functions
#include <ctype.h>   // character functions
#include <stdbool.h> // bool definitions
#include <nok5110LCD.h>
#include <LS7366R.h>
#include <linearAxis.h>
#include <usciA1Uart.h>


// list all commands
enum COMMAND_INDEX
{
    PIXEL, SCRNLINE, LINE, LCDCLEAR, LOADLS, READLS, WRITELS,
    CLEARLS, ZEROENC, COMPAREENC, AXISEN, RESCALE, TESTING, NUM_COMMANDS
};
CMD cmds[NUM_COMMANDS]={0};

const char *seps = "\t,\n ;:";       // for tokenizing the line string
unsigned char rxSpiBuff[] = "EmptyBuffer!";
unsigned char txSpiBuff[] = "EmptyBuffer!";
unsigned char cmdStr[100];
uint32_t LS7366Count = 0;
long int encoderCountMSP;

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

    cmds[LCDCLEAR].name = "LCDCLEAR";
    cmds[LCDCLEAR].nArgs = 0;
    cmds[LCDCLEAR].index = LCDCLEAR;

    cmds[LOADLS].name = "LOADLS";
    cmds[LOADLS].nArgs = 1;
    cmds[LOADLS].index = LOADLS;

    cmds[READLS].name = "READLS";
    cmds[READLS].nArgs = 1;
    cmds[READLS].index = READLS;

    cmds[WRITELS].name = "WRITELS";
    cmds[WRITELS].nArgs = 2;
    cmds[WRITELS].index = WRITELS;

    cmds[CLEARLS].name = "CLEARLS";
    cmds[CLEARLS].nArgs = 1;
    cmds[CLEARLS].index = CLEARLS;

    cmds[ZEROENC].name = "ZEROENC";
    cmds[ZEROENC].nArgs = 0;
    cmds[ZEROENC].index = ZEROENC;

    cmds[COMPAREENC].name = "COMPAREENC";
    cmds[COMPAREENC].nArgs = 0;
    cmds[COMPAREENC].index = COMPAREENC;

    cmds[AXISEN].name = "AXISEN";
    cmds[AXISEN].nArgs = 1;
    cmds[AXISEN].index = AXISEN;

    cmds[RESCALE].name = "RESCALE";
    cmds[RESCALE].nArgs = 1;
    cmds[RESCALE].index = RESCALE;

    cmds[TESTING].name = "TESTING";
    cmds[TESTING].nArgs = 2;
    cmds[TESTING].index = TESTING;


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
    unsigned int argIndex;

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
            //return -1;
        }
        else
        {
            if ((cmdNum==LOADLS) || (cmdNum==READLS) || (cmdNum==WRITELS) || (cmdNum==CLEARLS))
            {
                if (strcmp(tok ,"CNTR") == 0)
                    cmds[cmdNum].args[0] = CNTR;
                else if (strcmp(tok,"MDR0") == 0)
                    cmds[cmdNum].args[0] = MDR0;
                else if (strcmp(tok,"MDR1") == 0)
                    cmds[cmdNum].args[0] = MDR1;
                else if (strcmp(tok,"DTR") == 0)
                    cmds[cmdNum].args[0] = DTR;
                else if (strcmp(tok,"OTR") == 0)
                    cmds[cmdNum].args[0] = OTR;
                else if (strcmp(tok,"STR") == 0)
                    cmds[cmdNum].args[0] = STR;
                if (cmdNum==WRITELS){
                    cmds[cmdNum].args[1] = strtoul(tok,NULL,0);
                }
            }
            else
                cmds[cmdNum].args[argIndex] = atoi(tok);
        }
    }
    // check for too many argument
    if (strtok(NULL,seps)!=NULL){
        //ucsiA1UartTxString("ERROR! TOO MANY ARGUMENT!");
        //return -1;
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



    //All code to execute the commands go in here.
    switch (cmdIndex){
    case PIXEL:     // (x,y)
        x_start = cmds[PIXEL].args[0];
        y_start = cmds[PIXEL].args[1];

        if (x_start>LCD_MAX_COL || y_start>LCD_MAX_ROW){
            ucsiA1UartTxString(errMsg);
            return -1;
        }
        nokLcdSetPixel(x_start,y_start);
        break;

    case SCRNLINE:
        x_start = cmds[SCRNLINE].args[0];
        y_start = cmds[SCRNLINE].args[1];

        if (x_start>LCD_MAX_COL || y_start>LCD_MAX_ROW){
            ucsiA1UartTxString(errMsg);
            return -1;
        }
        line_mode = cmds[SCRNLINE].args[2];
        nokLcdDrawScrnLine(x_start,y_start,line_mode);
        break;

    case LINE:
        x_start = cmds[LINE].args[0];
        y_start = cmds[LINE].args[1];

        if (x_start>LCD_MAX_COL || y_start>LCD_MAX_ROW){
            ucsiA1UartTxString(errMsg);
            return -1;
        }
        x_end = cmds[LINE].args[2];
        y_end = cmds[LINE].args[3];
        if (x_end>LCD_MAX_COL || y_end>LCD_MAX_ROW){
            ucsiA1UartTxString(errMsg);
            return -1;
        }
        nokLcdDrawLine(x_start,y_start,x_end,y_end);
        break;

    case LCDCLEAR:
        nokLcdClear();
        break;

    case LOADLS:
        loadLS7366R(cmds[LOADLS].args[0]);
        break;

    case READLS:
        readLS7366R(cmds[READLS].args[0], rxSpiBuff);
        if ((cmds[READLS].args[0] == OTR) || (cmds[READLS].args[0] == CNTR))
            sprintf(cmdStr, "reg: %d %d %d %d", rxSpiBuff[3], rxSpiBuff[2], rxSpiBuff[1], rxSpiBuff[0]);
        else
            sprintf(cmdStr, "reg: %d", rxSpiBuff[0]);
        ucsiA1UartTxString(cmdStr);

        break;
    case WRITELS:
        txSpiBuff[0] = cmds[WRITELS].args[1];
        writeLS7366R(cmds[WRITELS].args[0], txSpiBuff);
        sprintf(cmdStr, "write: %x", cmds[WRITELS].args[1]);
        ucsiA1UartTxString(cmdStr);
        break;

    case CLEARLS:
        clearLS7366R(cmds[CLEARLS].args[0]);
        break;

    case ZEROENC:
        //clearLS7366R(CNTR);
        encoderCountMSP = 0;
        break;

    case COMPAREENC:
//        readLS7366R(CNTR, rxSpiBuff);
//        // LS7366Count shifting
//        LS7366Count = ( (uint32_t)rxSpiBuff[3]<<24 |
//                        (uint32_t)rxSpiBuff[2]<<16 |
//                        rxSpiBuff[1]<<8 |
//                        rxSpiBuff[0]);
//
//        sprintf(cmdStr, "LS7366R count: %lu", LS7366Count);
//        ucsiA1UartTxString(cmdStr);
        sprintf(cmdStr, "MSP430 count: %d", encoderCountMSP);
        ucsiA1UartTxString(cmdStr);
        break;

    case AXISEN:
        if (cmds[AXISEN].args[0])
            P1OUT &= ~LA_EN;
        else
            P1OUT |= LA_EN;
        break;

    case RESCALE:
        ucsiA1UartTxString("Updated scale");
        break;

    case TESTING:
        ucsiA1UartTxString("Testing");
        nokLcdDrawLinearAxisLine(cmds[TESTING].args[0],cmds[TESTING].args[1]);
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

unsigned char checkLSReg(unsigned char * reg)
{
    unsigned char cmdReg;
    if (reg == "CNTR")
        cmdReg = CNTR;
    else if (reg == "MDR0")
        cmdReg = MDR0;
    else if (reg == "MDR1")
        cmdReg = MDR1;
    else if (reg == "DTR")
        cmdReg = DTR;
    else if (reg == "OTR")
        cmdReg = OTR;
    else if (reg == "STR")
        cmdReg = STR;
    return cmdReg;
}

unsigned char getPWMScale(){
    return cmds[RESCALE].args[0];
}

