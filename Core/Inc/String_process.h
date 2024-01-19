/*
 * String-process.h
 *
 *  Created on: Dec 19, 2023
 *      Author: Admin
 */

#ifndef INC_STRING_PROCESS_H_
#define INC_STRING_PROCESS_H_

#include "stm32f1xx.h"

/*	@Function: isWordinBuff
 *  @Return : if the "word" is presented in the "databuff",
 *  			function return the pointer point to the first element
 *  			of "word" in "databuff"
 *  		else return NULL
 */
uint8_t* isWordinBuff(uint8_t *databuff,uint16_t buff_size, uint8_t *word);

uint16_t getBetween(uint8_t *firstWord,uint8_t *lastWord, uint8_t *buff, uint16_t size, uint8_t *getBuff);

uint16_t getRemainsize(uint8_t *Remainbuff, uint8_t *Originbuff, uint16_t originSize);

uint16_t cpyBuff2Buff(uint8_t *Destbuff, uint8_t *SourceBuff,uint16_t Sourcebuff_maxsize, uint16_t head, uint16_t tail);

uint8_t countnumbofOccurrent(char *str, char character );

void twobyte2buff(uint8_t *buffer, uint16_t number);

uint16_t buff2twobyte (uint8_t *buff);

void FourbytenumbertoBuff(uint32_t inputNumber, uint8_t *Buff);

uint32_t buff2Fourbyte(uint8_t *buff);
#endif /* INC_STRING_PROCESS_H_ */
