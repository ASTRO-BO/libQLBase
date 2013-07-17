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
#include "InputFileFilter.h"

namespace qlbase
{

InputFileFilter::InputFileFilter()
{
	row_status = 0;
	n_good_rows = 0;
}

InputFileFilter::~InputFileFilter()
{
}

bool InputFileFilter::Open(const std::string &filename) {
	status = 0;
	this->filename = filename;
	if ( !fits_open_table(&infptr, filename.c_str(), READONLY, &status) ) {
		opened = true;
	}
	else
		opened = false;

	if (status) {
		char* errms; std::string msg("Error in InputFileFilter::Open() ");
		fits_read_errmsg(errms);
		msg += 	errms;
		//gm.PrintLogMessage(msg, true);
		std::cerr << msg << std::endl;
		fits_report_error(stderr, status);
		status = 0;
		return false;
	}
	return true;
}

bool InputFileFilter::Close() {
	status = 0;
	fits_close_file(infptr, &status);
	if (status) {
		char* errms; std::string msg = "Error in InputFileFilter::Close() ";
		fits_read_errmsg(errms);
		msg += errms;
		//gm.PrintLogMessage(msg, true);
		std::cerr << msg << std::endl;
		fits_report_error(stderr, status);
		opened = true;
		status = 0;
		return false;
	}
	opened = false;
	return true;
}

bool InputFileFilter::MoveHeader(int header_number) {
	status = 0;
	fits_movabs_hdu(infptr, header_number + 1, 0, &status);
	if (status) {
		char* errms; std::string msg("Error in InputFileFilter::MoveHeader() ");
		//fits_read_errmsg(errms);
		//msg += errms;
		//gm.PrintLogMessage(msg, true);
		std::cerr << msg << std::endl;
		fits_report_error(stderr, status);
		status = 0;
		return false;
	}
	return true;
}

int InputFileFilter::GetColNum(char* nomecol) {
	int colnum = -1;
	fits_get_colnum(infptr,CASEINSEN,nomecol,&colnum,&status);
	if (status) {
		char* errms; std::string msg("Error in InputFileFilter::GetColNum() ");
		fits_read_errmsg(errms);
		msg += errms;
		//gm.PrintLogMessage(msg, true);
		std::cerr << msg << std::endl;
		fits_report_error(stderr, status);
		status = 0;
		return false;
	}
	return colnum;
}

double* InputFileFilter::ReadCol(int headernum, char* colname, int64_t frow, int64_t nrows) {

	if(!nrows)
		return 0;

	status = 0;
	int anynull;
	#ifdef READDOUBLE
	int typecode = TDOUBLE;
	#else
	int typecode = TFLOAT;
	#endif
	int felem = 1;
	double null = 0.0;

	delete[] row_status;
	row_status = (char*) new char[nrows];
	int colnum;
	bool res= MoveHeader(headernum);
	if(res == false)
		return 0;
	colnum = GetColNum(colname);
	if(colnum == -1)
		return 0;
	double* data = (double*) new double[nrows];
	fits_read_col(infptr, typecode, colnum, frow, felem, nrows, &null,  data, &anynull, &status);
	if (status) {
		char* errms; std::string msg("Error in InputFileFilter::ReadCol(): ");
		//fits_read_errmsg(errms);
		//msg += errms;
		//gm.PrintLogMessage(msg, true);
		std::cerr << msg << std::endl;
		fits_report_error(stderr, status);
		status = 0;
		return false;
	}
	return data;
}

}
