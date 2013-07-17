/*
 * Copyright (C) 2001 Andrea Bulgarelli
 *               2013 Andrea Zoli
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "InputFile.h"

namespace ql_io {

InputFile::InputFile() {
	opened = kFALSE;
	status = 0;
	applyFilter = kFALSE;
	selectEvent = "";
	headerBase = 1; //starting index = 0
}


InputFile::~InputFile() {

}

void InputFile::SetBaseHeader(int32_t headerBase) {
	this->headerBase = headerBase;
}


Bool_t InputFile::IsOpened() {
	return opened;
}

int64_t InputFile::GetNEvents() {
	MoveHeader(headerBase);
	return GetNRows();
}

}
