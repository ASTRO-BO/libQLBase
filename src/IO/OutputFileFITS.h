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

#ifndef QL_IO_OUTPUTFILEFITS_H
#define QL_IO_OUTPUTFILEFITS_H

#include <stdint.h>
#include <fitsio.h>
#include <stdexcept>
#include "OutputFile.h"

namespace qlbase {



class OutputFileFITS : public OutputFile {

public:

	OutputFileFITS();
	virtual ~OutputFileFITS();

	virtual void create(const std::string &filename);
	virtual void open(const std::string &filename);
	virtual void close();
	virtual bool isOpened() { return opened; }

	virtual void moveToHeader(int number);

	virtual void createTable(const std::string& name, const std::vector<field>& fields);
	virtual void writeu8i(int ncol, std::vector<uint8_t>& buff, long frow, long lrow);
	virtual void write16i(int ncol, std::vector<int16_t>& buff, long frow, long lrow);
	virtual void write32i(int ncol, std::vector<int32_t>& buff, long frow, long lrow);
	virtual void write64i(int ncol, std::vector<int64_t>& buff, long frow, long lrow);
	virtual void write32f(int ncol, std::vector<float>& buff, long frow, long lrow);
	virtual void write64f(int ncol, std::vector<double>& buff, long frow, long lrow);

private:

	bool opened;
	fitsfile *infptr;

	void throwException(const char *msg, int status);

	template<class T>
	void _write(int ncol, std::vector<T>& buff, int type, long frow, long lrow);

	const std::string _getFieldTypeString(fieldType type);
};

}

#endif
