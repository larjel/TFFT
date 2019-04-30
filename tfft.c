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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define TFFT_INCLUDE_USER_FILE_TABLE
#include "tfft.h"
#undef TFFT_INCLUDE_USER_FILE_TABLE

#if TFFT_USE_FILE_CRC16
#include "tfft_crc16.h"
#elif TFFT_USE_FILE_CRC8
#include "tfft_crc8.h"
#endif

/* Tiny Fixed File Table TFFT
 Fixed file system for small EEPROM and other non volatile memory where
 the user does not wish to use any space for a real file system or where
 a "real" file system simply is overkill.
 The reason I made this file system is basically that I often have
 found myself doing different kind of storage methods when saving simple data to EEPROM.
 Often this data is just some counters - perhaps some unsigned 32 bit numbers or similar.
 The EEPROM might be as small as 128 bytes and using a real file system is just not feasible.
 */

/* Macros */
#define TFFT_IS_ADDRESS_IN_RANGE(addr) (addr >= TFFT_START_ADDRESS && addr <= TFFT_END_ADDRESS)
#define TFFT_IS_FILE_NAME_ALLOWED(fname) (fname >= 0 && fname < TFFT_FILE_COUNT)

#define TFFT_GET_FILE_SIZE_WITH_CHECKSUM(fname) sa_fileTable[fname] + TFFT_USE_FILE_CRC8 + (TFFT_USE_FILE_CRC16 * 2)

static uint8_t saf_busy = 0;
static uint32_t sau32_errorCount = 0;

/*----------------------------------------------------------------------------*/
uint32_t TFFT_GetErrorCount()
{
  return sau32_errorCount;
}

/*----------------------------------------------------------------------------*/
void TFFT_ResetErrorCount()
{
  sau32_errorCount = 0;
}

/*----------------------------------------------------------------------------*/
static TFFT_SIZE_TYPE TFFT_GetTypeMaxFileSize(void)
{
  switch(sizeof(TFFT_SIZE_TYPE))
  {
      case 1:
        return (TFFT_SIZE_TYPE)0xFF;
      case 2:
        return (TFFT_SIZE_TYPE)0xFFFF;
      case 4:
        return (TFFT_SIZE_TYPE)0xFFFFFFFF;
      default:
        return 0;
  }
}

/*----------------------------------------------------------------------------*/
static TFFT_SIZE_TYPE TFFT_EepromStrlen(const char *pStr)
{
  TFFT_SIZE_TYPE len;
  TFFT_SIZE_TYPE maxLen = TFFT_GetTypeMaxFileSize();

  for(len = 0; len < maxLen && pStr[len] != '\0'; len++)
  {
  }

  return len;
}

/*----------------------------------------------------------------------------*/
/* Get file size with checksum and backup size (if used) */
inline static TFFT_ADDR_TYPE TFFT_GetRealFileSize(TFFT_FILE_NAME_TYPE fname)
{
  TFFT_ADDR_TYPE realSize = TFFT_GET_FILE_SIZE_WITH_CHECKSUM(fname);
#if TFFT_BACKUP_MODE_ENABLED
    realSize *= 2;
#endif // TFFT_BACKUP_MODE_ENABLED
  return realSize;
}

/*----------------------------------------------------------------------------*/
/* Calculate the address based on size of preceding data */
static TFFT_ADDR_TYPE TFFT_GetAddress(TFFT_FILE_NAME_TYPE fname)
{
  TFFT_FILE_NAME_TYPE i;
  TFFT_ADDR_TYPE address = TFFT_START_ADDRESS;

  for(i = 0; i < TFFT_FILE_COUNT; i++)
  {
    if(i == fname)
    {
      return address;
    }

    address += TFFT_GetRealFileSize(i);
  }

  return 0;
}

/*----------------------------------------------------------------------------*/
/* Use this to verify that the highest possible address fits chosen data type */
uint32_t TFFT_GetMaxAddress(void)
{
  TFFT_FILE_NAME_TYPE i;
  unsigned long address = (uint32_t)TFFT_START_ADDRESS;

  for(i = 0; i < TFFT_FILE_COUNT; i++)
  {
    address += TFFT_GetRealFileSize(i);
  }

  return (address - 1);
}

/*----------------------------------------------------------------------------*/
size_t TFFT_GetFileTableSize(void)
{
  return sizeof(sa_fileTable);
}

/*----------------------------------------------------------------------------*/
/* Read/Write byte from/to EEPROM */
static int TFFT_ReadWriteByte(TFFT_ADDR_TYPE address, uint8_t *pByte, uint8_t f_write, uint16_t *pChecksum)
{
  int rtnCode;

  if(TFFT_IS_ADDRESS_IN_RANGE(address))
  {
    if(f_write)
    {
      rtnCode = TFFT_EEPROM_WRITE_BYTE_FUNC(address, *pByte);
    }
    else // Read
    {
      rtnCode = TFFT_EEPROM_READ_BYTE_FUNC(address, pByte);
    }

    if(rtnCode != TFFT_RW_OK)
    {
      return rtnCode; // EEPROM Read/Write error
    }

#if TFFT_USE_FILE_CRC8 || TFFT_USE_FILE_CRC16
    if(pChecksum)
    {
#if TFFT_USE_FILE_CRC8
      TFFT_Crc8(*pByte, (uint8_t*)pChecksum);
#else // TFFT_USE_FILE_CRC16
      TFFT_Crc16(*pByte, pChecksum);
#endif
    }
#endif

    return TFFT_RW_OK; // Success
  }

  return TFFT_RW_ERR_ADDRESS; // Address out of range
}

#if TFFT_BACKUP_MODE_ENABLED
/*----------------------------------------------------------------------------*/
/* Read/Write file from/to EEPROM
   Returns either TFFT_RW_OK or a negative value
   indicating that an error occurred */
static int TFFT_ReadWriteFileInternal(TFFT_FILE_NAME_TYPE fname, TFFT_SIZE_TYPE size,
                         uint8_t *pData, uint8_t f_write, uint8_t f_truncate, uint8_t f_duplicate)
#else
static int TFFT_ReadWriteFileInternal(TFFT_FILE_NAME_TYPE fname, TFFT_SIZE_TYPE size,
                         uint8_t *pData, uint8_t f_write, uint8_t f_truncate)
#endif
{
  TFFT_ADDR_TYPE address;
  TFFT_ADDR_TYPE i;
  int rtnCode;

#if TFFT_USE_FILE_CRC8 || TFFT_USE_FILE_CRC16
  uint16_t u16Temp = 0;
  uint16_t checksum = 0;
#endif

  if(!TFFT_IS_FILE_NAME_ALLOWED(fname))
  {
    return(TFFT_RW_ERR_FILE_NAME); // File name not allowed
  }

  // Is the size of the requested file to store larger than
  // what has been reserved in the file table?
  if(size > sa_fileTable[fname])
  {
    if(f_write && !f_truncate)
    {
      return(TFFT_RW_ERR_FILE_TOO_LARGE); // Trying to write too large file
    }
    else // Write with truncated data or, if read, adjust length
    {
      size = sa_fileTable[fname];
    }
  }

  address = TFFT_GetAddress(fname);

#if TFFT_BACKUP_MODE_ENABLED
  // If the file to access is the duplicate (aka backup) file, then it will reside
  // after the first (primary) file.
  if(f_duplicate)
  {
    address += TFFT_GET_FILE_SIZE_WITH_CHECKSUM(fname);
  }
#endif // TFFT_BACKUP_MODE_ENABLED

  for(i = address; i < (address + size); i++)
  {
#if TFFT_USE_FILE_CRC8 || TFFT_USE_FILE_CRC16
    rtnCode = TFFT_ReadWriteByte(i, pData, f_write, &checksum);
#else
    rtnCode = TFFT_ReadWriteByte(i, pData, f_write, 0);
#endif

    if(rtnCode != TFFT_RW_OK)
    {
      return(rtnCode);
    }

    pData++;
  }

#if TFFT_USE_FILE_CRC8 || TFFT_USE_FILE_CRC16
  // Continue write or read so whole allocated memory
  // is used, otherwise the checksum won't be correct
  for( ; i < (address + size); i++)
  {
    rtnCode = TFFT_ReadWriteByte(i, (uint8_t*)&u16Temp, f_write, &checksum);

    if(rtnCode != TFFT_RW_OK)
    {
      return(rtnCode);
    }
  }

  // Write/Read checksum bytes
  if(f_write)
  {
#if TFFT_DEBUG_ENABLED
    printf("Write checksum = 0x%04X\n", (unsigned int)checksum);
#endif
    rtnCode = TFFT_ReadWriteByte(i, (uint8_t*)&checksum, f_write, 0);
#if TFFT_USE_FILE_CRC16
    rtnCode = TFFT_ReadWriteByte(i + 1, ((uint8_t*)&checksum) + 1, f_write, 0);
#endif
  }
  else // Read
  {
      rtnCode = TFFT_ReadWriteByte(i, (uint8_t*)&u16Temp, f_write, 0);
#if TFFT_USE_FILE_CRC16
      rtnCode = TFFT_ReadWriteByte(i + 1, ((uint8_t*)&u16Temp) + 1, f_write, 0);
#endif
#if TFFT_DEBUG_ENABLED
      printf("Read file checksum = 0x%04X\n", (unsigned int)u16Temp);
      printf("Read calculated checksum = 0x%04X\n", (unsigned int)checksum);
#endif
      if(u16Temp != checksum)
      {
        return(TFFT_RW_ERR_CHECKSUM); // Checksum error
      }
  }

  if(rtnCode != TFFT_RW_OK)
  {
    return(rtnCode); // Negative value
  }
#endif /* TFFT_USE_FILE_CRC8 || TFFT_USE_FILE_CRC16 */

  return TFFT_RW_OK;
}

#define TFFT_UPDATE_ERROR_COUNT(retVal) do{if((rtnVal)!=TFFT_RW_OK)sau32_errorCount++;}while(0)

/*----------------------------------------------------------------------------*/
/* Read/Write file from/to EEPROM
   Returns either TFFT_RW_OK or a negative value
   indicating that an error occurred */
int TFFT_ReadWriteFile(TFFT_FILE_NAME_TYPE fname, TFFT_SIZE_TYPE size,
                         uint8_t *pData, uint8_t f_write, uint8_t f_truncate)
{
  int rtnVal;

  //TODO: Checking and setting the busy flag should be a safe section
  if(saf_busy)
  {
    rtnVal = TFFT_RW_ERR_EEPROM_BUSY;
  }
  else
  {
    saf_busy = 1;
#if TFFT_BACKUP_MODE_ENABLED
    // Write/Read first copy
    rtnVal = TFFT_ReadWriteFileInternal(fname, size, pData, f_write, f_truncate, 0);
    TFFT_UPDATE_ERROR_COUNT(rtnVal);

    if(f_write)
    {
      // Write second copy (backup)
      rtnVal = TFFT_ReadWriteFileInternal(fname, size, pData, f_write, f_truncate, 1);
      TFFT_UPDATE_ERROR_COUNT(rtnVal);
    }
    else if(rtnVal != TFFT_RW_OK)
    {
#if TFFT_DEBUG_ENABLED
      printf("rtnVal = %d\n", rtnVal);
#endif
      // There was an error reading the first copy, read the backup copy.
      rtnVal = TFFT_ReadWriteFileInternal(fname, size, pData, f_write, f_truncate, 1);
      TFFT_UPDATE_ERROR_COUNT(rtnVal);
    }
#else
    rtnVal = TFFT_ReadWriteFileInternal(fname, size, pData, f_write, f_truncate);
    TFFT_UPDATE_ERROR_COUNT(rtnVal);
#endif // TFFT_BACKUP_MODE_ENABLED
    saf_busy = 0;
  }

  return rtnVal;
}

/*----------------------------------------------------------------------------*/
int TFFT_Write64(TFFT_FILE_NAME_TYPE fname, uint64_t data)
{
  return TFFT_ReadWriteFile(fname, sizeof(uint64_t), (uint8_t*)&data, 1, 0);
}

/*----------------------------------------------------------------------------*/
int TFFT_Write32(TFFT_FILE_NAME_TYPE fname, uint32_t data)
{
  return TFFT_ReadWriteFile(fname, sizeof(uint32_t), (uint8_t*)&data, 1, 0);
}

/*----------------------------------------------------------------------------*/
int TFFT_Write16(TFFT_FILE_NAME_TYPE fname, uint16_t data)
{
  return TFFT_ReadWriteFile(fname, sizeof(uint16_t), (uint8_t*)&data, 1, 0);
}

/*----------------------------------------------------------------------------*/
int TFFT_Write8(TFFT_FILE_NAME_TYPE fname, uint8_t data)
{
  return TFFT_ReadWriteFile(fname, sizeof(uint8_t), (uint8_t*)&data, 1, 0);
}

/*----------------------------------------------------------------------------*/
int TFFT_WriteFloat(TFFT_FILE_NAME_TYPE fname, float data)
{
  return TFFT_ReadWriteFile(fname, sizeof(float), (uint8_t*)&data, 1, 0);
}

/*----------------------------------------------------------------------------*/
int TFFT_WriteDouble(TFFT_FILE_NAME_TYPE fname, double data)
{
  return TFFT_ReadWriteFile(fname, sizeof(double), (uint8_t*)&data, 1, 0);
}

/*----------------------------------------------------------------------------*/
/* Write null-terminated string
Return number of written bytes. Too long strings will be truncated.
*/
int TFFT_WriteString(TFFT_FILE_NAME_TYPE fname, const char *pStr)
{
  // Also write null terminator if string is shorter than what fits in eeprom.
  // If length of string is same as what fits in eeprom, the null terminator will be excluded.
  return TFFT_ReadWriteFile(fname, (TFFT_EepromStrlen(pStr) + 1), (uint8_t*)pStr, 1, 1);
}

/*----------------------------------------------------------------------------*/
/* Read null-terminated string from EEPROM
Return number of read bytes.
Note: Output buffer must be able to hold maxStrLen + 1 (for null terminator)
*/
int TFFT_ReadString(TFFT_FILE_NAME_TYPE fname, TFFT_SIZE_TYPE maxStrLen, char *pStr)
{
  // Insert null terminator at first position in case no string to be read
  pStr[0] = '\0';
  // Insert null terminator at maxStrLen (guarantees that string will be terminated)
  pStr[maxStrLen] = '\0';

  return TFFT_ReadWriteFile(fname, maxStrLen, (uint8_t*)pStr, 0, 0);
}
