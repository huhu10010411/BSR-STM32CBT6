/*
 * String_process.c
 *
 *  Created on: Dec 19, 2023
 *      Author: Admin
 */


#include "String_process.h"
#include <string.h>


uint8_t* isWordinBuff(uint8_t *databuff,uint16_t buff_size, uint8_t *word)
{
	uint16_t wordlen = strlen((char*)word);
	if (wordlen > buff_size )	{
		return NULL;
	}
	for (uint16_t i = 0; i< buff_size - wordlen + 1; i++)
	{
		if ( memcmp(databuff +i,word,wordlen) == 0)
		{
			return databuff+ i;
		}
	}
	return NULL;
}

uint16_t getBetween(uint8_t *firstWord,uint8_t *lastWord, uint8_t *buff, uint16_t size, uint8_t *getBuff)
{
	uint8_t fWlen = strlen((char*)firstWord);

	uint8_t *Fp =isWordinBuff(buff, size, firstWord);

	if (Fp == NULL ) return 0;
	Fp += fWlen;
	uint8_t remainSize = getRemainsize(Fp, buff, size);
	uint8_t *Lp = isWordinBuff(Fp, remainSize, lastWord);
	if (Lp == NULL || Fp >= Lp) return 0;

	uint8_t getBuffindex =0;
	while (Fp != Lp)
	{
		getBuff[getBuffindex++] = *Fp;
		Fp ++;
	}
	getBuff[getBuffindex]= '\0';
	return getBuffindex;           // length of getBuff
}

uint16_t getRemainsize(uint8_t *Remainbuff, uint8_t *Originbuff, uint16_t originSize)
{
	return originSize-(Remainbuff - Originbuff)/sizeof(uint8_t);
}

uint16_t cpyBuff2Buff(uint8_t *Destbuff, uint8_t *SourceBuff,uint16_t Sourcebuff_maxsize, uint16_t head, uint16_t tail)
{
	uint16_t size = 0;
	if (head == tail )
	{
		return 0;
	}
	else if (head > tail)
	{
		size = head -tail;
		memcpy(Destbuff, SourceBuff+tail, size);
	}
	else
	{
		size = Sourcebuff_maxsize - tail + head;
		memcpy(Destbuff, SourceBuff + tail, Sourcebuff_maxsize - tail);
		memcpy (Destbuff+ Sourcebuff_maxsize - tail, SourceBuff, head);
	}
	return size;
}

uint8_t countnumbofOccurrent(char *str, char character )
{
	uint8_t count = 0;
	for (uint16_t i = 0; i < strlen(str); i++)
	{
		if (str[i] == character) {
			count ++;
		}
	}
	return count;
}

void twobyte2buff(uint8_t *buffer, uint16_t number)
{
	buffer[0] = ( number >> 8 ) & 0xFF;
	buffer[1] = number & 0xFF;
}
uint16_t buff2twobyte (uint8_t *buff)
{
	uint16_t value = 0;
	value = buff[0] << 8 | buff[1];
	return value;
}

void FourbytenumbertoBuff(uint32_t inputNumber, uint8_t *Buff)
{
    uint32_t tmp =inputNumber;
	for (uint8_t i=0 ;i< 4;i++)
	{
		Buff[3-i] = tmp & (0xFF);
 		tmp = tmp >>8;
	}
}

uint32_t buff2Fourbyte(uint8_t *buff)
{
	uint32_t value = 0;
	value = buff[0] << 24 | buff[1] << 16 | buff[2] << 8 | buff[3];
	return value;
}
