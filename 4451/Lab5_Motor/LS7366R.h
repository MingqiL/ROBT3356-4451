/*
 * LS7366R.h
 *
 *  Created on: 2024Äê3ÔÂ15ÈÕ
 *      Author: leeqi
 */

#ifndef LS7366R_H_
#define LS7366R_H_

#define LS_SS BIT7

#define MDR0 BIT3
#define MDR1 BIT4
#define DTR (BIT4 | BIT3)
#define CNTR BIT5
#define OTR (BIT5 | BIT3)
#define STR (BIT5 | BIT4)

#define WR BIT7
#define RD BIT6
#define CLR 0x00
#define LOAD (BIT7 | BIT6)

void initLS7366R();
void clearLS7366R(unsigned char reg);
void readLS7366R(unsigned char reg, unsigned char* rxLS7366R);
void writeLS7366R(unsigned char reg, unsigned char* txLS7366R);
void loadLS7366R(unsigned char reg);

#endif /* LS7366R_H_ */

