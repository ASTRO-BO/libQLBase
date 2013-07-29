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

#ifndef QL_IO_OUTPUTFILE_H
#define QL_IO_OUTPUTFILE_H

#include <string>
#include <stdint.h>
#include <vector>
#include "File.h"

namespace qlbase {

enum fieldType
{
	UNSIGNED_INT8,
	INT16,
	INT32,
	INT64,
	FLOAT,
	DOUBLE
};

struct field
{
	std::string name;
	fieldType type;
	std::string unit;
};

/** The interface for reading from a generic file divided into chunks.
 * It specialize File adding functions for reading tables and images.
 */
class OutputFile : public File {

public:

	OutputFile() {}

	virtual ~OutputFile() {}

	/* Table chunk functions */
	virtual void createTable(const std::string& name, const std::vector<field>& fields) = 0;
	virtual void writeu8i(int ncol, std::vector<uint8_t>& buff, long frow, long lrow) = 0;
	virtual void write16i(int ncol, std::vector<int16_t>& buff, long frow, long lrow) = 0;
	virtual void write32i(int ncol, std::vector<int32_t>& buff, long frow, long lrow) = 0;
	virtual void write64i(int ncol, std::vector<int64_t>& buff, long frow, long lrow) = 0;
	virtual void write32f(int ncol, std::vector<float>& buff, long frow, long lrow) = 0;
	virtual void write64f(int ncol, std::vector<double>& buff, long frow, long lrow) = 0;
};

}

#endif
