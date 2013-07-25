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

#ifndef QL_IO_INPUTFILE_H
#define QL_IO_INPUTFILE_H

#include <string>
#include <stdint.h>
#include <vector>
#include "File.h"

namespace qlbase {

/** The interface for reading from a generic file divided into chunks.
 * It specialize File adding functions for reading tables and images.
 */
class InputFile : public File {

	public:

		InputFile() {}

		virtual ~InputFile() {}

		/* Table chunk functions */
		virtual int32_t getNCols() = 0;
		virtual int64_t getNRows() = 0;

		virtual std::vector<int8_t> read8i(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;
		virtual std::vector<int16_t> read16i(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;
		virtual std::vector<int32_t> read32i(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;
		virtual std::vector<int64_t> read64i(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;
		virtual std::vector<float> read32f(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;
		virtual std::vector<double> read64f(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;
};

}

#endif
