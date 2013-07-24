/*
 * Copyright (C) 2013 Andrea Zoli
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

#ifndef QL_IO_DEFINITION_H
#define QL_IO_DEFINITION_H

#include<iostream>

namespace qlbase
{

#define QL_DEBUG 1

#ifdef QL_DEBUG

#define DEBUG(msg) {std::cout << __FILE__ << ":" << __LINE__ << ":" << __func__ << ": " << msg << std::endl;}
#else
#define DEBUG(msg)
#endif

#define ERR(msg) {std::cerr << msg << std::endl;}

enum FileError {
    kFileNoError = 0,
    kFileNoRead,
    kEndFile,
    kErrorMoveHeader,
    kFileStopped,
    kFileDataError,
    kReadFileDisabled,
    kFileErrorRowsTerminated = 307
};

}

#endif
