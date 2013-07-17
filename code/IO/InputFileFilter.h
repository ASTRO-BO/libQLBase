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

#ifndef QL_IO_INPUTFILEFILTER_H
#define QL_IO_INPUTFILEFILTER_H

#include <string>
#include <cfitsio/fitsio.h>

namespace qlbase
{

class InputFileFilter {
public:
	InputFileFilter();

	virtual ~InputFileFilter();

	bool Open(const std::string &filename);

	bool Close();

	virtual bool Calculate(int64_t frow, int64_t nrows) = 0;

	virtual int64_t GetNGoodRows() { return n_good_rows; };

	virtual char* GetRowStatus() { return row_status; };

protected:

	virtual bool MoveHeader(int32_t header_number);

	virtual int32_t GetColNum(char* nomecol);

	virtual double* ReadCol(int32_t headernum, char* colname, int64_t frow, int64_t nrows);

	fitsfile *infptr;

	int64_t n_good_rows;

	char* row_status;

	int32_t status;

	std::string filename;

	bool opened;

};

}

#endif
