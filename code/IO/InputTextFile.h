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

#ifndef QL_IO_INPUTTEXTFILE_H
#define QL_IO_INPUTTEXTFILE_H

#include <fstream>
#include <cstdint>
#include "InputFile.h"

namespace ql_io {

class InputTextFile : public InputFile {

	public:

		InputTextFile(const std::string &separator = std::string(" "));

		virtual ~InputTextFile();

		bool Open(const std::string &filename);

		bool Close();

		bool MoveHeader(int header_number) { return true; };

		int64_t GetNextRowPeriod(uint32_t timeColumnNumber, int64_t pos_first, double end_time);

		double GetTime(uint32_t timeColumnNumber, uint64_t pos);

		bool IsOpened();

		double* GetTime(uint32_t timeColumnNumber, uint64_t start, uint64_t dim);

		uint8_t* Read_TBYTE(int ncol, long frow, long lrow, int64_t nelements = 0);

		int16_t* Read_TSHORT(int ncol, long frow, long lrow, int64_t nelements = 0);

		int32_t* Read_TINT(int ncol, long frow, long lrow, int64_t nelements = 0);

		int64_t* Read_TINT32BIT(int ncol, long frow, long lrow, int64_t nelements = 0);

		uint16_t* Read_TUSHORT(int ncol, long frow, long lrow, int64_t nelements = 0);

		uint32_t* Read_TUINT(int ncol, long frow, long lrow, int64_t nelements = 0);

		uint64_t* Read_TULONG(int ncol, long frow, long lrow, int64_t nelements = 0);

		float* Read_TFLOAT(int ncol, long frow, long lrow, int64_t nelements = 0);

		double* Read_TDOUBLE(int ncol, long frow, long lrow, int64_t nelements = 0);

		virtual uint32_t GetIndexFirstColumn() { return 0;};

		virtual uint32_t GetIndexFirstRow() { return 0;};

		virtual uint32_t GetIndexFirstTableHeader() { return 0; };

	protected:
		std::ifstream fileStream;
		std::string separator;

	protected:
		void pointTo(int line);
		bool findField(std::string& line, int& first, int& last, int pos = 0);
		bool reopen();
		bool test(int ncol, long frow, long& lrow);

	private:
		void _printState();
};

}

#endif
