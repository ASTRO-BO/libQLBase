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

#include "QLDefinition.h"
#include <string>
#include <cstdint>

namespace ql_io {

class InputFileFilter;

//##ModelId=3FAF8D8C034F
//##Documentation
//## This class represent a generic input file with data organized as tabular data.
//## The responsibilities of this class are:
//## - read each column from a row to another row
//## L'indice della prima colonna, l'indice della prima riga e l'indice del primo header partono tutti
//## da 0. Utilizzare i rispettivi metodi per gli offset.
//## $Date: 2007/05/23 17:24:46 $
//## $Header: /home/repository/cvs/gtb/libQL2/code/InputFile.h,v 1.2 2007/05/23 17:24:46 bulgarelli Exp $
//## $Id: InputFile.h,v 1.2 2007/05/23 17:24:46 bulgarelli Exp $
//## $Revision: 1.2 $
//## \brief Insert here a brief description of the class

class InputFile {

	public:

		InputFile( );

		virtual ~InputFile();

		//il base header è l'header su cui devono essere contati il numero di eventi di un file
		virtual void SetBaseHeader(int32_t headerBase);

		//##ModelId=3FAF8E5F0371
		virtual Bool_t Open(const std::string &filename) = 0;

		//##ModelId=3FAF8E850235
		virtual Bool_t Close() = 0;

		//##Documentation
		//## Si sposta nell'header specificato. La numerazione degli header parte da 0.
		//## Specificare l'offset con il metdo GetIndexFirstTableHeader().
		virtual Bool_t MoveHeader(int header_number) = 0;

		//##Documentation
		//## Apply a filter to the current header
		virtual Bool_t ApplyFilter(const std::string &string selectEvent) { return kFALSE; };

		//##Documentation
		//## Remove the filter to the current header
		virtual void RemoveFilter() {};


		virtual int32_t GetNCols() { return ncols; };

		virtual int64_t GetNRows() { return nrows; };

		virtual int64_t GetNEvents();

		virtual int64_t GetNextRowPeriod(uint32_t timeColumnNumber, int64_t pos_first, DOUBLE_T end_time) = 0;

		virtual DOUBLE_T* GetTime(uint32_t timeColumnNumber, uint64_t start, uint64_t dim) = 0;

		virtual DOUBLE_T GetTime(uint32_t timeColumnNumber, uint64_t pos) = 0;

		virtual Bool_t IsOpened();

		virtual int32_t GetStatus() { return status; };

		virtual std::string GetFileName() { return filename; };

		virtual void SetFileName(const std::string &filename) { this->filename = filename; };

		//##Documentation
		//## Indica da dove deve partire l'indice della prima colonna nel file (0 o 1)
		virtual uint32_t GetIndexFirstColumn() = 0;

		//##Documentation
		//## Indica da dove deve partire l'indice della prima riga nel file (0 o 1)
		virtual uint32_t GetIndexFirstRow() = 0;

		//##Documentation
		//## Indica da dove deve partire l'indice del primo header nei file strutturati in tabella (0 o 1)
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

		//##ModelId=3FAF8EE401B9
		std::string filename;

		Bool_t opened;

		//##Documentation
		//## Number of columns of the current file
		int32_t ncols;

		//##Documentation
		//## Number of rows of the current file
		int64_t nrows;

		//##Documentation
		//## Number of rows of the last reading;
		int64_t nRowsRead;

		//##Documentation
		//## The status of the reading
		int32_t status;

		Bool_t applyFilter;

		std::string selectEvent;

		//the header che contiene gli eventi da elaborare. Usato da GetNEvents()
		int32_t headerBase;

};

}

#endif
