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

#include <iostream>

#include "Definitions.h"
#include "InputFileFITS.h"

namespace qlbase {

#define IFFMAXROWSTATUS 1000000
#define ERRMSGSIZ 81

InputFileFITS::InputFileFITS() : InputFile() {
}

InputFileFITS::~InputFileFITS() {
}

void InputFileFITS::throwException(const char *msg, int status) {
	std::string errMsg(msg);

	char errDesc[ERRMSGSIZ];
	fits_read_errmsg(errDesc);
	errMsg += errDesc;

	throw IOException(msg, status);
}

bool InputFileFITS::Open(const std::string &filename) {
	int status = 0;
	this->filename = filename;
	if ( !fits_open_table(&infptr, filename.c_str(), READONLY, &status) ) {
		opened = true;
		fits_get_num_rows(infptr, &nrows, &status);
		fits_get_num_cols(infptr, &ncols, &status);
		DEBUG("InputFileFITS::Open() - " << filename << " - col: " << ncols << ", row: " << nrows << " - opened ");
	}
	else
		opened = false;

	if (status)
		throwException("Error in InputFileFITS::Open() ", status);

	return opened;
}

int32_t InputFileFITS::GetNCols() {
	int status = 0;
	fits_get_num_cols(infptr, &ncols, &status);

	if (status)
		throwException("Error in InputFileFITS::GetNCols() ", status);

	return ncols;
}

int64_t InputFileFITS::GetNRows() {
	int status = 0;
	fits_get_num_rows(infptr, &nrows, &status);

	if (status)
		throwException("Error in InputFileFITS::GetNRows() ", status);

	return nrows;
}

//##ModelId=3FAF8E850235
bool InputFileFITS::Close() {
	int status = 0;
	fits_close_file(infptr, &status);

	if (status)
		throwException("Error in InputFileFITS::Close() ", status);

	return true;
}

bool InputFileFITS::MoveHeader(int header_number) {
	status = 0;
	fits_movabs_hdu(infptr, header_number + GetIndexFirstTableHeader(), 0, &status);

	if (status)
		throwException("Error in InputFileFITS::MoveHeader() ", status);

	return true;
}

int64_t InputFileFITS::GetNextRowPeriod(uint32_t timeColumnNumber, int64_t pos_first, double end_time) {
	double* first_time = Read_TDOUBLE(timeColumnNumber, pos_first, pos_first);
	if(first_time == 0) return 0;
	//double end_time = (*first_time) + deltaT;
	double current_time = *first_time;
	int64_t next_pos = pos_first;
	while(current_time < end_time) {
		next_pos++;
		double* new_time = Read_TDOUBLE(timeColumnNumber, next_pos, next_pos);
		if(new_time == 0) return 0;
		current_time = *new_time;
		delete[] new_time;
	}
	next_pos --;
	delete[] first_time;
	return next_pos;
}

double InputFileFITS::GetTime(uint32_t timeColumnNumber, uint64_t pos) {
	MoveHeader(headerBase);
	double* new_time = Read_TDOUBLE(timeColumnNumber , pos+ GetIndexFirstRow(), pos+ GetIndexFirstRow());
	double ret = *new_time;
	delete[] new_time;
	return ret;
}

double* InputFileFITS::GetTime(uint32_t timeColumnNumber, uint64_t start, uint64_t dim) {
	MoveHeader(headerBase);
	double* ret = Read_TDOUBLE(timeColumnNumber , start+ GetIndexFirstRow(), start+dim-1+ GetIndexFirstRow());
	return ret;
}

uint8_t* InputFileFITS::Read_TBYTE(int ncol, long frow, long lrow, int64_t nelements) {
	status = 0;
	int anynull;
	int typecode = TBYTE;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	uint8_t* data = (uint8_t*) new uint8_t[nelem];
	//ncol: number of column
	//frow: number of row
	//felem: number of the first element to be read starting from the cell
	//identified by the (ncol, frow)
	//nelem: number of elements read. If the format if fixed it corresponds with
	//nelem cells starting from (ncol, frow) cell. If the format is variable or the
	//cell contains an array, nelem corresponds with the number of the data reads
	//into this cell.
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);
	if (status) {
		delete [] data;
		nRowsRead = 0;
		throwException("Error in InputFileFITS::Read_TBYTE() ", status);
	}

	nRowsRead = lrow - frow + 1;

	return data;
}

int16_t* InputFileFITS::Read_TSHORT(int ncol, long frow, long lrow, int64_t nelements) {
	status = 0;
	int anynull;
	int typecode = TSHORT;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	int16_t* data = (int16_t*) new int16_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);

	if (status) {
		delete [] data;
		nRowsRead = 0;
		throwException("Error in InputFileFITS::Read_TSHORT() ", status);
	}

	nRowsRead = lrow - frow + 1;

	return data;
}

int32_t* InputFileFITS::Read_TINT(int ncol, long frow, long lrow, int64_t nelements) {
	status = 0;
	int anynull;
	int typecode = TINT;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	int32_t* data = new int32_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);
	if (status) {
		delete [] data;
		nRowsRead = 0;
		throwException("Error in InputFileFITS::Read_TINT() ", status);
	}
	nRowsRead = lrow - frow + 1;

	return data;
}

int64_t* InputFileFITS::Read_TINT32BIT(int ncol, long frow, long lrow, int64_t nelements) {
	status = 0;
	int anynull;
	int typecode = TINT32BIT;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	int64_t* data = new int64_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);

	if (status) {
		delete [] data;
		nRowsRead = 0;
		throwException("Error in InputFileFITS::Read_TINT32BIT() ", status);
	}

	nRowsRead = lrow - frow + 1;

	return data;
}

uint16_t* InputFileFITS::Read_TUSHORT(int ncol, long frow, long lrow, int64_t nelements) {
	status = 0;
	int anynull;
	int typecode = TUSHORT;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	uint16_t* data = new uint16_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);

	if (status) {
		delete [] data;
		nRowsRead = 0;
		throwException("Error in InputFileFITS::Read_TUSHORT() ", status);
	}

	nRowsRead = lrow - frow + 1;

	return data;
}

uint32_t* InputFileFITS::Read_TUINT(int ncol, long frow, long lrow, int64_t nelements) {
	status = 0;
	int anynull;
	int typecode = TUINT;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	uint32_t* data = new uint32_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);

	if (status) {
		delete [] data;
		nRowsRead = 0;
		throwException("Error in InputFileFITS::Read_TUINT() ", status);
	}

	nRowsRead = lrow - frow + 1;

	return data;
}

uint64_t* InputFileFITS::Read_TULONG(int ncol, long frow, long lrow, int64_t nelements) {
	status = 0;
	int anynull;
	int typecode = TULONG;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	uint64_t* data = (uint64_t*) new uint64_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);

	if (status) {
		delete [] data;
		nRowsRead = 0;
		throwException("Error in InputFileFITS::Read_TULONG() ", status);
	}

	nRowsRead = lrow - frow + 1;

	return data;
}

float* InputFileFITS::Read_TFLOAT(int ncol, long frow, long lrow, int64_t nelements) {
	status = 0;
	int anynull;
	int typecode = TFLOAT;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	float null = 0.0;

	float* data = new float[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);

	if (status) {
		delete [] data;
		nRowsRead = 0;
		throwException("Error in InputFileFITS::Read_TFLOAT() ", status);
	}

	nRowsRead = lrow - frow + 1;

	return data;
}

double* InputFileFITS::Read_TDOUBLE(int ncol, long frow, long lrow, int64_t nelements) {
	status = 0;
	int anynull;
	int typecode = TDOUBLE;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	double null = 0.0;

	double* data = (double*) new double[nelem];
	//cout << "(2) ncol " << ncol << " frow " << frow << " nelem " << nelem << std::endl;
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);

	if (status) {
		delete [] data;
		nRowsRead = 0;
		throwException("Error in InputFileFITS::Read_TDOUBLE() ", status);
	}

	nRowsRead = lrow - frow + 1;

	return data;
}

}
