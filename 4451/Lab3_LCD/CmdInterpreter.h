/*
 * UsciCmdInterpreter.h
 *
 *  Created on: Jan. 9, 2022
 *      Author: Andrew Friesen
 */

#ifndef CMDINTERPRETER_H_
#define CMDINTERPRETER_H_

#define MAX_ARGS 4
#define MAX_CMDS 4
#define MAX_CMDS_LENGTH 255

typedef struct CMD {
    const char *name; //command name
    int nArgs; //number of input args
    int args[MAX_ARGS]; //args
    int index;
}CMD;


void cmdsinit();


int parseCmd(char * cmdLine);


int validateCmd(char * cmdName);


int executeCMD(int cmdIndex);

void makeStringUpperCase(char *str);

#endif /* CMDINTERPRETER_H_ */
