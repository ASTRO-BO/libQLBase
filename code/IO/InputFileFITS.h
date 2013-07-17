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

#ifndef QL_IO_INPUTFILEFITS_H
#define QL_IO_INPUTFILEFITS_H

#include <cstdint>
#include <cfitsio/fitsio.h>
#include "InputFile.h"

namespace qlbase {

class InputFileFITS : public InputFile {

	public:

		InputFileFITS();

		~InputFileFITS();

		virtual bool Open(const std::string &filename);

		virtual int32_t GetNCols();

		virtual int64_t GetNRows();

		virtual bool Close();

		virtual bool MoveHeader(int header_number);

		/** Apply a filter to the current header. */
		virtual bool ApplyFilter(const std::string &selectEvent);

		/** Remove the filter to the current header. */
		virtual void RemoveFilter();

		virtual int64_t GetNextRowPeriod(uint32_t timeColumnNumber, int64_t pos_first, double end_time);

		virtual double* GetTime(uint32_t timeColumnNumber, uint64_t start, uint64_t dim);

		virtual double GetTime(uint32_t timeColumnNumber, uint64_t pos);

		virtual uint32_t GetIndexFirstColumn() { return 1;};

		virtual uint32_t GetIndexFirstRow() { return 1;};

		virtual uint32_t GetIndexFirstTableHeader() { return 1; };

		virtual void SetFilter(InputFileFilter* filter);

	public:

		/** Read a FITS column.
         *
		 * Example 1 (fixed):
		 * uint8_t* gridlcdi = inputFile->Read_TBYTE(GRIDLCDI, first_pos_new, last_pos_new);
		 * if(gridlcdi == kPointerNull) return inputFile->GetStatus();
         *
		 * Example 2 (variable):
		 * uint8_t* aclatcon = tempFile.Read_TBYTE(ACLATCON, j+1, j+1, 12);
		 * if(aclatcon == kPointerNull)
		 *     return tempFile.GetStatus();
         *
		 * @param ncol Number of column to be read.
		 * @param frow Number of the first row to be read (in the case of fixed
		 *        format) or number of the row to be read (in the case of
		 *        variable format).
		 * @param lrow Number of the last row to be read (in the case of fixed
		 *        format) or not used in the case of variable format.
		 * @param nelements Not used in the case of fixed format (it become
		 *        equal to lrow - frow + 1) or number of the elements reads into
		 *        the cell identified by the (ncol, frow) in the case of
		 *        variable format.
         */
		virtual uint8_t* Read_TBYTE(int ncol, long frow, long lrow, int64_t nelements = 0);

		virtual int16_t* Read_TSHORT(int ncol, long frow, long lrow, int64_t nelements = 0);

		virtual int32_t* Read_TINT(int ncol, long frow, long lrow, int64_t nelements = 0);

		virtual int64_t* Read_TINT32BIT(int ncol, long frow, long lrow, int64_t nelements = 0);

		virtual uint16_t* Read_TUSHORT(int ncol, long frow, long lrow, int64_t nelements = 0);

		virtual uint32_t* Read_TUINT(int ncol, long frow, long lrow, int64_t nelements = 0);

		virtual uint64_t* Read_TULONG(int ncol, long frow, long lrow, int64_t nelements = 0);

		virtual float* Read_TFLOAT(int ncol, long frow, long lrow, int64_t nelements = 0);

		virtual double* Read_TDOUBLE(int ncol, long frow, long lrow, int64_t nelements = 0);

		fitsfile *infptr;

	protected:

		virtual bool GetFilteredRows(int64_t frow, int64_t nrows);

		InputFileFilter* filter;

		long n_good_rows;

		char* row_status;

		int64_t row_status_size;

};

}

#endif
