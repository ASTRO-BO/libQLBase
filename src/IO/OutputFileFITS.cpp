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

#include <iostream>
#include <cstring>

#include "Definitions.h"
#include "OutputFileFITS.h"

namespace qlbase {


#define ERRMSGSIZ 81

OutputFileFITS::OutputFileFITS() : opened(false) {
}

OutputFileFITS::~OutputFileFITS() {
}

void OutputFileFITS::throwException(const char *msg, int status) {
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

void OutputFileFITS::create(const std::string &filename) {
	int status = 0;

	fits_create_file(&infptr, filename.c_str(), &status);

	if (status)
		throwException("Error in OutputFileFITS::create() ", status);

	opened = true;
}

void OutputFileFITS::open(const std::string &filename) {
	int status = 0;

	fits_open_table(&infptr, filename.c_str(), READWRITE, &status);

	if (status)
		throwException("Error in OutputFileFITS::open() ", status);

	opened = true;
}

void OutputFileFITS::close() {
	int status = 0;
	fits_close_file(infptr, &status);

	if (status)
		throwException("Error in OutputFileFITS::close() ", status);

	opened = false;
}

void OutputFileFITS::jumpToChunk(int number) {
	int status = 0;

	if(!isOpened())
		throwException("Error in OutputFileFITS::jumpToChuck() ", status);

	fits_movabs_hdu(infptr, number+1, 0, &status);

	if (status)
		throwException("Error in OutputFileFITS::jumpToChuck() ", status);
}

void OutputFileFITS::createTable(const std::string& name, const std::vector<field>& fields) {
	int status = 0;

	if(!isOpened())
		throwException("Error in OutputFileFITS::createTable() ", status);

	unsigned int nfields = fields.size();

	char** ttypes = new char*[nfields*sizeof(char*)];
	char** tform = new char*[nfields*sizeof(char*)];
	char** tunit = new char*[nfields*sizeof(char*)];

	for(unsigned int i=0; i<nfields; i++)
	{
		ttypes[i] = new char[10];
		std::string type = fields[i].name.c_str();
		std::memcpy(ttypes[i], type.c_str(), 10);

		tform[i] = new char[10];
		std::string form = _getFieldTypeString(fields[i].type);
		std::memcpy(tform[i], form.c_str(), 10);

		tunit[i] = new char[10];
		std::string unit = fields[i].unit.c_str();
		std::memcpy(tunit[i], unit.c_str(), 10);
	}

	fits_create_tbl(infptr, BINARY_TBL, nfields, 0, ttypes, tform, tunit, name.c_str(), &status);

	for(unsigned int i=0; i<nfields; i++)
	{
		delete ttypes[i];
		delete tform[i];
		delete tunit[i];
	}
	delete ttypes;
	delete tform;
	delete tunit;

	if (status)
		throwException("Error in OutputFileFITS::jumpToChuck() ", status);
}


void OutputFileFITS::writeu8i(int ncol, std::vector<uint8_t>& buff, long frow, long lrow)
{
	_write(ncol, buff, TBYTE, frow, lrow);
}

void OutputFileFITS::write16i(int ncol, std::vector<int16_t>& buff, long frow, long lrow)
{
	_write(ncol, buff, TSHORT, frow, lrow);
}

void OutputFileFITS::write32i(int ncol, std::vector<int32_t>& buff, long frow, long lrow)
{
	_write(ncol, buff, TINT32BIT, frow, lrow);
}

void OutputFileFITS::write64i(int ncol, std::vector<int64_t>& buff, long frow, long lrow)
{
	_write(ncol, buff, TLONG, frow, lrow);
}

void OutputFileFITS::write32f(int ncol, std::vector<float>& buff, long frow, long lrow)
{
	_write(ncol, buff, TFLOAT, frow, lrow);
}

void OutputFileFITS::write64f(int ncol, std::vector<double>& buff, long frow, long lrow)
{
	_write(ncol, buff, TDOUBLE, frow, lrow);
}

template<class T>
void OutputFileFITS::_write(int ncol, std::vector<T>& buff, int type, long frow, long lrow) {
	int status = 0;
	if(!isOpened())
		throwException("Error in OutputFileFITS::write() ", status);

	long nelem = lrow - frow + 1;

	std::vector<T> buffptr = buff;
	fits_write_col(infptr, type, ncol+1, frow+1, 1, nelem, &buffptr[0], &status);

	if(status)
		throwException("Error in OutputFileFITS::write() ", status);
}


const std::string OutputFileFITS::_getFieldTypeString(fieldType type) {
	switch(type)
	{
		case UNSIGNED_INT8:
			return "rB";
		case INT16:
			return "rI";
		case INT32:
			return "rJ";
		case INT64:
			return "rK";
		case FLOAT:
			return "rE";
		case DOUBLE:
			return "rD";
	}
	return "";
}

}
