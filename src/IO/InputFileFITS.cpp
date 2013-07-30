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

#define ERRMSGSIZ 81

InputFileFITS::InputFileFITS() : opened(false) {
}

InputFileFITS::~InputFileFITS() {
}

void InputFileFITS::throwException(const char *msg, int status) {
	std::string errMsg(msg);

	if(status != 0)
	{
		char errDesc[ERRMSGSIZ];
		fits_read_errmsg(errDesc);
		errMsg += errDesc;
	}
	else
		errMsg += "Reading from a closed file.";

	throw IOException(msg, status);
}

void InputFileFITS::open(const std::string &filename) {
	int status = 0;
	fits_open_table(&infptr, filename.c_str(), READONLY, &status);

	if (status)
		throwException("Error in InputFileFITS::Open() ", status);

	opened = true;
}

void InputFileFITS::close() {
	int status = 0;
	fits_close_file(infptr, &status);

	if (status)
		throwException("Error in InputFileFITS::Close() ", status);

	opened = false;
}

void InputFileFITS::jumpToChunk(int number) {
	int status = 0;

	if(!isOpened())
		throwException("Error in InputFileFITS::jumpToChuck() ", status);

	fits_movabs_hdu(infptr, number+1, 0, &status);

	if (status)
		throwException("Error in InputFileFITS::jumpToChuck() ", status);
}

int32_t InputFileFITS::getNCols() {
	int status = 0;

	if(!isOpened())
		throwException("Error in InputFileFITS::getNCols() ", status);

	int32_t ncols;
	fits_get_num_cols(infptr, &ncols, &status);

	if (status)
		throwException("Error in InputFileFITS::getNCols() ", status);

	return ncols;
}

int64_t InputFileFITS::getNRows() {
	int status = 0;

	if(!isOpened())
		throwException("Error in InputFileFITS::getNRows() ", status);

	int64_t nrows;
	fits_get_num_rows(infptr, &nrows, &status);

	if (status)
		throwException("Error in InputFileFITS::getNRows() ", status);

	return nrows;
}

std::vector<uint8_t> InputFileFITS::readu8i(int ncol, long frow, long lrow) {
	std::vector<uint8_t> buff;
	read(ncol, buff, TBYTE, frow, lrow);
	return buff;
}

std::vector<int16_t> InputFileFITS::read16i(int ncol, long frow, long lrow) {
	std::vector<int16_t> buff;
	read(ncol, buff, TSHORT, frow, lrow);
	return buff;
}

std::vector<int32_t> InputFileFITS::read32i(int ncol, long frow, long lrow) {
	std::vector<int32_t> buff;
	read(ncol, buff, TINT, frow, lrow);
	return buff;
}

std::vector<int64_t> InputFileFITS::read64i(int ncol, long frow, long lrow) {
	std::vector<int64_t> buff;
	read(ncol, buff, TLONG, frow, lrow);
	return buff;
}

std::vector<float> InputFileFITS::read32f(int ncol, long frow, long lrow) {
	std::vector<float> buff;
	read(ncol, buff, TFLOAT, frow, lrow);
	return buff;
}

std::vector<double> InputFileFITS::read64f(int ncol, long frow, long lrow) {
	std::vector<double> buff;
	read(ncol, buff, TDOUBLE, frow, lrow);
	return buff;
}

template<class T>
void InputFileFITS::read(int ncol, std::vector<T>& buff, int type, long frow, long lrow) {
	int status = 0;
	if(!isOpened())
		throwException("Error in InputFileFITS::read() ", status);

	int anynull;
	long nelem = lrow - frow + 1;
	long null = 0;

	buff.resize(nelem);

	fits_read_col(infptr, type, ncol+1, frow+1, 1, nelem, &null,  &buff[0], &anynull, &status);

	if(status)
		throwException("Error in InputFileFITS::read() ", status);
}

}
