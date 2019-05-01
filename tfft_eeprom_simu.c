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
 * @file tfft_eeprom_simu.c
 * @brief File containing
 *
 * Here typically goes a more extensive explanation of what the header defines.
 *
 * @author Lars Jelleryd
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "tfft.h"

/* For eeprom simulation */
static uint8_t simEeprom[2048];

/*----------------------------------------------------------------------------*/
/* This should be an external platform specific function
   The function may use return codes 0, -10 and lower than -20 for user defined errors
   -10 should be used for generic write failure and other codes may be used for
   more specific error codes.
   Return: 0 (TFFT_RW_OK) = write OK, -10 (TFFT_RW_ERR_LOW_LEVEL_WRITE) = write failed */
int TFFT_EepromWriteByte(TFFT_ADDR_TYPE address, uint8_t byte)
{
  simEeprom[address] = byte;

  return TFFT_RW_OK; // Write OK
}

/*----------------------------------------------------------------------------*/
/* This should be an external platform specific function
   The function may use return codes 0, -11 and lower than -20 for user defined errors
   -11 should be used for generic read failure and other codes may be used for
   more specific error codes.
   Return: 0 (TFFT_RW_OK) = read OK, -11 (TFFT_RW_ERR_LOW_LEVEL_READ) = read failed */
int TFFT_EepromReadByte(TFFT_ADDR_TYPE address, uint8_t *pByte)
{
  *pByte = simEeprom[address];

  return TFFT_RW_OK; // Read OK
}

/*----------------------------------------------------------------------------*/
/* Print the content of the "EEPROM" */
void TFFT_EepromPrintMemory(TFFT_ADDR_TYPE addrStart, TFFT_ADDR_TYPE addrEnd)
{
  TFFT_ADDR_TYPE i;

  for(i = addrStart; i <= addrEnd; i++)
  {
    if(i % 16 == 0)
    {
      printf("\n");
    }
    printf("%02X ", (unsigned int)simEeprom[i]);
  }

}
