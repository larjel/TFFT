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
 * @file tfft_eeprom_simu.h
 * @brief File containing
 *
 * Here typically goes a more extensive explanation of what the header defines.
 *
 * @author Lars Jelleryd
 */

#ifndef TFFT_EEPROM_SIMU_H_
#define TFFT_EEPROM_SIMU_H_

int TFFT_EepromWriteByte(TFFT_ADDR_TYPE address, uint8_t byte);
int TFFT_EepromReadByte(TFFT_ADDR_TYPE address, uint8_t *pByte);
void TFFT_EepromPrintMemory(TFFT_ADDR_TYPE addrStart, TFFT_ADDR_TYPE addrEnd);

#endif /* TFFT_EEPROM_SIMU_H_ */
