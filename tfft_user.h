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
#ifndef TFFT_USER_H_
#define TFFT_USER_H_

//=========================================================
// START: User defines (ALL data types must be unsigned)
//=========================================================
/** Address data type. Holds EEPROM address (is not used in file table and
will only save memory in functions) */
#define TFFT_ADDR_TYPE uint16_t
/** File size data type. Must be able to hold maximum file size (in bytes).
Used in file table. Smaller type = smaller file table. */
#define TFFT_SIZE_TYPE uint8_t
/** File "name" data type. Must be able to hold maximum number of files to be
used (is not used in file table and will only save memory in functions) */
#define TFFT_FILE_NAME_TYPE uint8_t

/** Set to 1 if one byte CRC8 should be used, else to 0 */
#define TFFT_USE_FILE_CRC8 1
/** Set to 1 if two bytes CRC16 should be used, else to 0 */
#define TFFT_USE_FILE_CRC16 0

/** First writable address in EEPROM. */
#define TFFT_START_ADDRESS    16
/** Last writable address in EEPROM. */
#define TFFT_END_ADDRESS    2047

/** Set to 1 to enable backup mode. If backup mode is enabled, the file will
be stored twice for safety. If the primary file would fail in checksum verification,
the backup will be used. This also means that it is of no use to enable this mode
unless CRC8 or CRC16 is enabled. (CRC16 is recommended.) Also note that this mode
will use twice as much space in the EEPROM! */
#define TFFT_BACKUP_MODE_ENABLED 0

/** Set to 1 to enable printf debug messages */
#define TFFT_DEBUG_ENABLED 1

//----- END: User defines -----------------

//=========================================================
// START: User Read and Write EEPROM functions
//=========================================================

// Include the user supplied (platform specific) EEPROM read and write functions
#include "tfft_eeprom_simu.h"

// Set function names

/** This should be an external platform specific function
   The function may use return codes 0, -10 and < -20 for user defined errors
   -10 should be used for generic write failure and other codes may be used for
   more specific error codes. See examples in tfft_eeprom_simu.c
   Return: 0 (TFFT_RW_OK) = write OK, -10 (TFFT_RW_ERR_LOW_LEVEL_WRITE) = write failed */
#define TFFT_EEPROM_WRITE_BYTE_FUNC    TFFT_EepromWriteByte

/** This should be an external platform specific function
   The function may use return codes 0, -11 and < -20 for user defined errors
   -11 should be used for generic read failure and other codes may be used for
   more specific error codes. See examples in tfft_eeprom_simu.c
   Return: 0 (TFFT_RW_OK) = read OK, -11 (TFFT_RW_ERR_LOW_LEVEL_READ) = read failed */
#define TFFT_EEPROM_READ_BYTE_FUNC     TFFT_EepromReadByte

//----- END: User Read and Write EEPROM functions ------

//=======================================
// START: File setup
//=======================================
// File "names". Should match the positions in the file table.
enum
{
  FILE0_NAME_EEPROM_FILE_VERSION_U8,
  FILE1_NAME_SENSOR_VAL1_U32,
  FILE2_NAME_TEXT_LABEL1_STR10,
  FILE3_NAME_SENSOR_VAL2_S32,
  TFFT_FILE_COUNT // Number of files. MUST always be at the end.
};

// Files sizes (optional). Used for buffer allocation in user code, e.g. char buf[FILE2_SIZE_STR10+1]
#define FILE2_SIZE_STR10 10
//------- END: File setup -------------

// This define is used to make sure that only tfft.c includes the file table.
#ifdef TFFT_INCLUDE_USER_FILE_TABLE

//=======================================
// START: File table setup
//=======================================
const static TFFT_SIZE_TYPE sa_fileTable[TFFT_FILE_COUNT] =
{
    sizeof(uint8_t),   // FILE0_NAME_EEPROM_FILE_VERSION_U8
    sizeof(uint32_t),  // FILE1_NAME_SENSOR_VAL1_U32
    FILE2_SIZE_STR10,  // FILE2_NAME_TEXT_LABEL1_STR10
    sizeof(int32_t)    // FILE3_NAME_SENSOR_VAL2_S32
};
//------- END: File table setup -------

#endif /* TFFT_INCLUDE_USER_FILE_TABLE */

#endif /* TFFT_USER_H_ */
