/****************************************************************************
 *  Copyright (C) 2013-2019 by Lars Jelleryd                                *
 *                                                                          *
 *  This file is part of Tiny Fixed File Table (TFFT).                     *
 *                                                                          *
 *  TFFT is free software: you can redistribute it and/or modify it         *
 *  under the terms of the GNU Lesser General Public License as published   *
 *  by the Free Software Foundation, either version 3 of the License, or    *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  TFFT is distributed in the hope that it will be useful,                 *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *  GNU Lesser General Public License for more details.                     *
 *                                                                          *
 *  You should have received a copy of the GNU Lesser General Public        *
 *  License along with TFFT.  If not, see <http://www.gnu.org/licenses/>.   *
 ****************************************************************************/

/**
 * @file tfft.h
 * @brief File containing
 *
 * Here typically goes a more extensive explanation of what the header defines.
 *
 * @author Lars Jelleryd
 */

#ifndef TFFT_H_
#define TFFT_H_

#include "tfft_user.h"

// Return codes for writing/reading
#define TFFT_RW_OK                    0 // Success
#define TFFT_RW_ERR_FILE_NAME        -1 // File name not allowed
#define TFFT_RW_ERR_FILE_TOO_LARGE   -2 // Trying to write too large file
#define TFFT_RW_ERR_ADDRESS          -3 // Address out of range
#define TFFT_RW_ERR_CHECKSUM         -4 // CRC error
#define TFFT_RW_ERR_FILE_TABLE       -5 // File table is corrupt
#define TFFT_RW_ERR_LOW_LEVEL_WRITE -10 // Low level write failed
#define TFFT_RW_ERR_LOW_LEVEL_READ  -11 // Low level read failed
#define TFFT_RW_ERR_EEPROM_BUSY     -12 // EEPROM currently busy. Try later.

#if(TFFT_USE_FILE_CRC8 && TFFT_USE_FILE_CRC16)
#error TFFT_USE_FILE_CRC8 and TFFT_USE_FILE_CRC16 are mutually exclusive!
#endif

uint32_t TFFT_GetMaxAddress(void);
size_t TFFT_GetFileTableSize(void);
uint32_t TFFT_GetErrorCount();
void TFFT_ResetErrorCount();

int TFFT_ReadWriteFile(TFFT_FILE_NAME_TYPE fname, TFFT_SIZE_TYPE size, uint8_t *pData, uint8_t f_write, uint8_t f_truncate);

int TFFT_Write64(TFFT_FILE_NAME_TYPE fname, uint64_t data);
int TFFT_Write32(TFFT_FILE_NAME_TYPE fname, uint32_t data);
int TFFT_Write16(TFFT_FILE_NAME_TYPE fname, uint16_t data);
int TFFT_Write8(TFFT_FILE_NAME_TYPE fname, uint8_t data);

int TFFT_WriteFloat(TFFT_FILE_NAME_TYPE fname, float data);
int TFFT_WriteDouble(TFFT_FILE_NAME_TYPE fname, double data);

#define TFFT_WriteData(fname, size, pSrc) TFFT_ReadWriteFile(fname, size, (u8*)pSrc, 1, 0)

/**
 * @brief Write unsigned 64 bit datatype
 * @param fname File name
 * @param data Unsigned 64 bit number to write
 * @return Result code. See return codes from TFFT_ReadWriteFile().
 */
#define TFFT_WriteU64(fname, data) TFFT_Write64(fname, data)
#define TFFT_WriteS64(fname, data) TFFT_Write64(fname, (uint64_t)data)
#define TFFT_WriteU32(fname, data) TFFT_Write32(fname, data)
#define TFFT_WriteS32(fname, data) TFFT_Write32(fname, (uint32_t)data)
#define TFFT_WriteU16(fname, data) TFFT_Write16(fname, data)
#define TFFT_WriteS16(fname, data) TFFT_Write16(fname, (uint16_t)data)
#define TFFT_WriteU8(fname, data) TFFT_Write8(fname, data)
#define TFFT_WriteS8(fname, data) TFFT_Write8(fname, (uint8_t)data)
#define TFFT_WriteChar(fname, data) TFFT_Write8(fname, (uint8_t)data)

#define TFFT_ReadData(fname, size, pDest) TFFT_ReadWriteFile(fname, size, (uint8_t*)pDest, 0, 0)

#define TFFT_ReadU64(fname, pDest) TFFT_ReadWriteFile(fname, sizeof(uint64_t), (uint8_t*)pDest, 0, 0)
#define TFFT_ReadS64(fname, pDest) TFFT_ReadWriteFile(fname, sizeof(int64_t), (uint8_t*)pDest, 0, 0)
#define TFFT_ReadU32(fname, pDest) TFFT_ReadWriteFile(fname, sizeof(uint32_t), (uint8_t*)pDest, 0, 0)
#define TFFT_ReadS32(fname, pDest) TFFT_ReadWriteFile(fname, sizeof(int32_t), (uint8_t*)pDest, 0, 0)
#define TFFT_ReadU16(fname, pDest) TFFT_ReadWriteFile(fname, sizeof(uint16_t), (uint8_t*)pDest, 0, 0)
#define TFFT_ReadS16(fname, pDest) TFFT_ReadWriteFile(fname, sizeof(int16_t), (uint8_t*)pDest, 0, 0)
#define TFFT_ReadU8(fname, pDest) TFFT_ReadWriteFile(fname, sizeof(uint8_t), (uint8_t*)pDest, 0, 0)
#define TFFT_ReadChar(fname, pDest) TFFT_ReadWriteFile(fname, sizeof(char), (uint8_t*)pDest, 0, 0)
#define TFFT_ReadFloat(fname, pDest) TFFT_ReadWriteFile(fname, sizeof(float), (uint8_t*)pDest, 0, 0)
#define TFFT_ReadDouble(fname, pDest) TFFT_ReadWriteFile(fname, sizeof(double), (uint8_t*)pDest, 0, 0)

int TFFT_WriteString(TFFT_FILE_NAME_TYPE fname, const char *pStr);
int TFFT_ReadString(TFFT_FILE_NAME_TYPE fname, TFFT_SIZE_TYPE maxStrLen, char *pStr);

const char* TFFT_RetValToStr(int retVal);

#endif /* TFFT_H_ */
