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
#include <cstdint>

namespace ql_io {

class InputFileFilter;

/** This class represent a generic input file with data organized as tabular
 *  data.
 */
class InputFile {

	public:

		InputFile( );

		virtual ~InputFile();

		virtual void SetBaseHeader(int32_t headerBase);

		virtual bool Open(const std::string &filename) = 0;

		virtual bool Close() = 0;

		virtual bool MoveHeader(int header_number) = 0;

		/** Apply a filter to the current header */
		virtual bool ApplyFilter(const std::string &string selectEvent) { return false; };

		/** Remove the filter to the current header */
		virtual void RemoveFilter() {};

		virtual int32_t GetNCols() { return ncols; };

		virtual int64_t GetNRows() { return nrows; };

		virtual int64_t GetNEvents();

		virtual int64_t GetNextRowPeriod(uint32_t timeColumnNumber, int64_t pos_first, double end_time) = 0;

		virtual double* GetTime(uint32_t timeColumnNumber, uint64_t start, uint64_t dim) = 0;

		virtual double GetTime(uint32_t timeColumnNumber, uint64_t pos) = 0;

		virtual bool IsOpened();

		virtual int32_t GetStatus() { return status; };

		virtual std::string GetFileName() { return filename; };

		virtual void SetFileName(const std::string &filename) { this->filename = filename; };

		virtual uint32_t GetIndexFirstColumn() = 0;

		virtual uint32_t GetIndexFirstRow() = 0;

		virtual uint32_t GetIndexFirstTableHeader() = 0;

		virtual void SetFilter(InputFileFilter* filter) {};

	public:

		virtual int64_t GetNRowReads() { return nRowsRead; };

		virtual uint8_t* Read_TBYTE(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;

		virtual int16_t* Read_TSHORT(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;

		virtual int32_t* Read_TINT(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;

		virtual int64_t* Read_TINT32BIT(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;

		virtual uint16_t* Read_TUSHORT(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;

		virtual uint32_t* Read_TUINT(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;

		virtual uint64_t* Read_TULONG(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;

		virtual float* Read_TFLOAT(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;

		virtual double* Read_TDOUBLE(int ncol, long frow, long lrow, int64_t nelements = 0) = 0;

	protected:

		std::string filename;

		bool opened;

		int32_t ncols;
		int64_t nrows;

		/** Number of rows of the last reading. */
		int64_t nRowsRead;

		/** The status of the reading. */
		int32_t status;

		bool applyFilter;

		std::string selectEvent;

		int32_t headerBase;
};

}

#endif
