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

void InputFileFITS::open(const std::string &filename) {
	int status = 0;
	fits_open_table(&infptr, filename.c_str(), READONLY, &status);

	if (status)
		throwException("Error in InputFileFITS::Open() ", status);
}

void InputFileFITS::close() {
	int status = 0;
	fits_close_file(infptr, &status);

	if (status)
		throwException("Error in InputFileFITS::Close() ", status);
}

void InputFileFITS::jumpToChunk(int number) {
	int status = 0;
	fits_movabs_hdu(infptr, number + GetIndexFirstTableHeader(), 0, &status);

	if (status)
		throwException("Error in InputFileFITS::jumpToChuck() ", status);
}

int32_t InputFileFITS::getNCols() {
	int status = 0;
	int32_t ncols;
	fits_get_num_cols(infptr, &ncols, &status);

	if (status)
		throwException("Error in InputFileFITS::getNCols() ", status);

	return ncols;
}

int64_t InputFileFITS::getNRows() {
	int status = 0;
	int64_t nrows;
	fits_get_num_rows(infptr, &nrows, &status);

	if (status)
		throwException("Error in InputFileFITS::getNRows() ", status);

	return nrows;
}

std::vector<int8_t> InputFileFITS::read8i(int ncol, long frow, long lrow, int64_t nelements) {
	int status = 0;
	int anynull;
	int typecode = TBYTE;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	//ncol: number of column
	//frow: number of row
	//felem: number of the first element to be read starting from the cell
	//identified by the (ncol, frow)
	//nelem: number of elements read. If the format if fixed it corresponds with
	//nelem cells starting from (ncol, frow) cell. If the format is variable or the
	//cell contains an array, nelem corresponds with the number of the data reads
	//into this cell.
	std::vector<int8_t> data;
	data.resize(nelem);
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  &data[0], &anynull, &status);
	if (status) {
		throwException("Error in InputFileFITS::read() ", status);
	}

	return data;
}

std::vector<int16_t> InputFileFITS::read16i(int ncol, long frow, long lrow, int64_t nelements) {
	int status = 0;
	int anynull;
	int typecode = TSHORT;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	std::vector<int16_t> data;
	data.resize(nelem);
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  &data[0], &anynull, &status);

	if (status)
		throwException("Error in InputFileFITS::read16i() ", status);

	return data;
}

std::vector<int32_t> InputFileFITS::read32i(int ncol, long frow, long lrow, int64_t nelements) {
	int status = 0;
	int anynull;
	int typecode = TINT;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	std::vector<int32_t> data;
	data.resize(nelem);
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  &data[0], &anynull, &status);
	if (status)
		throwException("Error in InputFileFITS::read32i() ", status);

	return data;
}

std::vector<int64_t> InputFileFITS::read64i(int ncol, long frow, long lrow, int64_t nelements) {
	int status = 0;
	int anynull;
	int typecode = TLONG;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	long null = 0;

	std::vector<int64_t> data;
	data.resize(nelem);
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  &data[0], &anynull, &status);

	if (status)
		throwException("Error in InputFileFITS::read64i() ", status);

	return data;
}

std::vector<float> InputFileFITS::read32f(int ncol, long frow, long lrow, int64_t nelements) {
	int status = 0;
	int anynull;
	int typecode = TFLOAT;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	float null = 0.0;

	std::vector<float> data;
	data.resize(nelem);
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  &data[0], &anynull, &status);

	if (status)
		throwException("Error in InputFileFITS::read32f() ", status);

	return data;
}

std::vector<double> InputFileFITS::read64f(int ncol, long frow, long lrow, int64_t nelements) {
	int status = 0;
	int anynull;
	int typecode = TDOUBLE;
	int felem = 1;
	long nelem;
	if(nelements == 0)
		nelem = lrow - frow + 1;
	else
		nelem = nelements;
	double null = 0.0;

	std::vector<double> data;
	data.resize(nelem);
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  &data[0], &anynull, &status);

	if (status)
		throwException("Error in InputFileFITS::read64f() ", status);

	return data;
}

}
