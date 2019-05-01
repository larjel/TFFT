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
 * @file main.c
 * @brief File containing.
 *
 * Here typically goes a more extensive explanation of what the header defines.
 *
 * @author Lars Jelleryd
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "tfft.h"
#include "tfft_eeprom_simu.h"

static void MAIN_ChkRetVal(int retVal)
{
    const char *pRetValStr = TFFT_RetValToStr(retVal);
    printf("Status = %s\n", pRetValStr);
}

int main()
{
  uint32_t u32TestRead = 0;

  uint8_t u8TestRead = 0;

  int32_t s32TestRead = 0;

  int16_t s16TestRead = 0;

  double doubleTestRead = 0.0;

  char szRead[FILE2_SIZE_STR10 + 1];

  printf("EEPROM Super Simple File System!\n");

  printf("Max address: %lu [ < 256 = u8, < 65536 = u16 else u32 ]\n", (unsigned long)TFFT_GetMaxAddress());
  printf("File table size: %u\n", (unsigned int)TFFT_GetFileTableSize());

  // ### WRITE ###
  printf("\n## Write u8: 1\n");
  MAIN_ChkRetVal(TFFT_WriteU8(FILE0_NAME_EEPROM_FILE_VERSION_U8, (uint8_t)1));

  printf("\n## Write u32: 4578686\n");
  MAIN_ChkRetVal(TFFT_WriteU32(FILE1_NAME_SENSOR_VAL1_U32, (uint32_t)4578686));

  printf("\n## Write string: %s\n", "1234567892");
  MAIN_ChkRetVal(TFFT_WriteString(FILE2_NAME_TEXT_LABEL1_STR10, "1234567892-----------"));

  printf("\n## Write s32: -34567890\n");
  MAIN_ChkRetVal(TFFT_WriteS32(FILE3_NAME_SENSOR_VAL2_S32, (int32_t)-34567890));

  printf("\n## Write s32: -34567890\n");
  MAIN_ChkRetVal(TFFT_WriteS32(45, (int32_t)-34567890));

  // ### READ BACK ###
  printf("\n$$ Read u8\n");
  MAIN_ChkRetVal(TFFT_ReadU8(FILE0_NAME_EEPROM_FILE_VERSION_U8, &u8TestRead));
  printf("u8: %u\n", (unsigned int)u8TestRead);

  printf("\n$$ Read u32\n");
  MAIN_ChkRetVal(TFFT_ReadU32(FILE1_NAME_SENSOR_VAL1_U32, &u32TestRead));
  printf("u32: %u\n", (unsigned int)u32TestRead);

  printf("\n$$ Read string\n");
  MAIN_ChkRetVal(TFFT_ReadString(FILE2_NAME_TEXT_LABEL1_STR10, FILE2_SIZE_STR10, szRead));
  printf("string: %s\n", szRead);

  printf("\n$$ Read s32\n");
  MAIN_ChkRetVal(TFFT_ReadS32(FILE3_NAME_SENSOR_VAL2_S32, &s32TestRead));
  printf("s32: %d\n", (int)s32TestRead);
  //-----------------

  TFFT_EepromPrintMemory(0, 255);

  return 0;
}
