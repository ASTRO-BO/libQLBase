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
#include "InputFileFITS.h"
#include "File.h"

using namespace std;

namespace ql_io {

#define IFFMAXROWSTATUS 1000000

InputFileFITS::InputFileFITS() : InputFile() {
	row_status = 0;
	row_status = (char*) new char[IFFMAXROWSTATUS];
	row_status_size = IFFMAXROWSTATUS;
}

InputFileFITS::~InputFileFITS() {
	delete[] row_status;
}

bool InputFileFITS::Open(const std::string &filename) {
	File file;
	if(file.Open(filename, "r"))
		file.Close();
	else
		return false;
	opened = true;
	status = 0;
	this->filename = filename;
	if ( !fits_open_table(&infptr, filename, READONLY, &status) ) {
		opened = true;
		fits_get_num_rows(infptr, &nrows, &status);
		fits_get_num_cols(infptr, &ncols, &status);
		PD("InputFileFITS::Open() - " << filename << " - col: " << ncols << ", row: " << nrows << " - opened ");
	}
	else
		opened = kFALSE;

	if (status) {
		char* errms; std::string msg("Error in InputFileFITS::Open() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		status = 0;
		return kFALSE;
	}
	return opened;
}

int32_t InputFileFITS::GetNCols() {
	status = 0;
	fits_get_num_cols(infptr, &ncols, &status);
	if (status) {
		char* errms; std::string  msg("Error in InputFileFITS::GetNCols() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		status = 0;
		return 0;
	}
	return ncols;
}

int64_t InputFileFITS::GetNRows() {
	status = 0;
	fits_get_num_rows(infptr, &nrows, &status);
	if (status) {
		char* errms; std::string msg("Error in InputFileFITS::GetNRows() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		status = 0;
		return 0;
	}
	return nrows;
}

//##ModelId=3FAF8E850235
bool InputFileFITS::Close() {
	status = 0;
	fits_close_file(infptr, &status);
	if (status) {
		char* errms; std::string msg("Error in InputFileFITS::Close() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		opened = kTRUE;
		status = 0;
		return kFALSE;
	}
	opened = kFALSE;
	return kTRUE;
}

bool InputFileFITS::MoveHeader(int header_number) {
	status = 0;
	fits_movabs_hdu(infptr, header_number + GetIndexFirstTableHeader(), 0, &status);
	if (status) {
		char* errms; std::string msg("Error in InputFileFITS::MoveHeader() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		status = 0;
		return false;
	}
	return true;
}

bool InputFileFITS::ApplyFilter(const std::string &selectEvent) {
	applyFilter = kTRUE;
	this->selectEvent = selectEvent;
}

void InputFileFITS::RemoveFilter() {
	applyFilter = kFALSE;
}

int64_t InputFileFITS::GetNextRowPeriod(uint32_t timeColumnNumber, int64_t pos_first, DOUBLE_T end_time) {
	DOUBLE_T* first_time = READ_TDOUBLE(timeColumnNumber, pos_first, pos_first);
	if(first_time == 0) return 0;
	//DOUBLE_T end_time = (*first_time) + deltaT;
	DOUBLE_T current_time = *first_time;
	int64_t next_pos = pos_first;
	while(current_time < end_time) {
		next_pos++;
		DOUBLE_T* new_time = READ_TDOUBLE(timeColumnNumber, next_pos, next_pos);
		if(new_time == 0) return 0;
		current_time = *new_time;
		delete[] new_time;
	}
	next_pos --;
	delete[] first_time;
	return next_pos;
}

DOUBLE_T InputFileFITS::GetTime(uint32_t timeColumnNumber, uint64_t pos) {
	MoveHeader(headerBase);
	DOUBLE_T* new_time = READ_TDOUBLE(timeColumnNumber , pos+ GetIndexFirstRow(), pos+ GetIndexFirstRow());
	DOUBLE_T ret = *new_time;
	delete[] new_time;
	return ret;
}

DOUBLE_T* InputFileFITS::GetTime(uint32_t timeColumnNumber, uint64_t start, uint64_t dim) {
	MoveHeader(headerBase);
	DOUBLE_T* ret = READ_TDOUBLE(timeColumnNumber , start+ GetIndexFirstRow(), start+dim-1+ GetIndexFirstRow());
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
	if(applyFilter) {
		if(GetFilteredRows(frow, nelem)) {
			if(n_good_rows == 0) {
				nRowsRead = 0;
				return 0;
			}
		} else {
			nRowsRead = 0;
			return 0;
		}
	}
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
		char* errms; std::string msg("Error in InputFileFITS::Read_TBYTE() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		return 0;
	}
	nRowsRead = lrow - frow + 1;
	if(applyFilter) {
		if(n_good_rows > 0) {
			uint8_t* dataf = (uint8_t*) new uint8_t[n_good_rows];
			long j = 0;
			for(long i=0; i<nelem; i++)
				if(row_status[i] == 1)
					dataf[j++] = data[i];
			delete[] data;
			data = dataf;
			nRowsRead = n_good_rows;
		} else {
			delete[] data;
			data = 0;
			nRowsRead = 0;
		}
	}
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
	if(applyFilter) {
		if(GetFilteredRows(frow, nelem)) {
			if(n_good_rows == 0) {
				nRowsRead = 0;
				return 0;
			}
		} else {
			nRowsRead = 0;
			return 0;
		}
	}
	int16_t* data = (int16_t*) new int16_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);
	if (status) {
		delete [] data;
		nRowsRead = 0;
		char* errms; std::string msg("Error in InputFileFITS::Read_TSHORT() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		return 0;
	}
	nRowsRead = lrow - frow + 1;
	if(applyFilter) {
		if(n_good_rows > 0) {
			int16_t* dataf = (int16_t*) new int16_t[n_good_rows];
			long j = 0;
			for(long i=0; i<nelem; i++)
				if(row_status[i] == 1)
					dataf[j++] = data[i];
			delete[] data;
			data = dataf;
			nRowsRead = n_good_rows;
		} else {
			delete[] data;
			data = 0;
			nRowsRead = 0;
		}
	}
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
	if(applyFilter) {
		if(GetFilteredRows(frow, nelem)) {
			if(n_good_rows == 0) {
				nRowsRead = 0;
				return 0;
			}
		} else {
			nRowsRead = 0;
			return 0;
		}
	}
	int32_t* data = new int32_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);
	if (status) {
		delete [] data;
		nRowsRead = 0;
		char* errms; std::string msg("Error in InputFileFITS::Read_TINT() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		return 0;
	}
	nRowsRead = lrow - frow + 1;
	if(applyFilter) {
		if(n_good_rows > 0) {
			int32_t* dataf = (int32_t*) new int32_t[n_good_rows];
			long j = 0;
			for(long i=0; i<nelem; i++)
				if(row_status[i] == 1)
					dataf[j++] = data[i];
			delete[] data;
			data = dataf;
			nRowsRead = n_good_rows;
		} else {
			delete[] data;
			data = 0;
			nRowsRead = 0;
		}
	}
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
	if(applyFilter) {
		if(GetFilteredRows(frow, nelem)) {
			if(n_good_rows == 0) {
				nRowsRead = 0;
				return 0;
			}
		} else {
			nRowsRead = 0;
			return 0;
		}
	}
	int64_t* data = new int64_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);
	if (status) {
		delete [] data;
		nRowsRead = 0;
		char* errms; std::string msg("Error in InputFileFITS::Read_TINT32BIT() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		return 0;
	}
	nRowsRead = lrow - frow + 1;
	if(applyFilter) {
		if(n_good_rows > 0) {
			int64_t* dataf = (int64_t*) new int64_t[n_good_rows];
			long j = 0;
			for(long i=0; i<nelem; i++)
				if(row_status[i] == 1)
					dataf[j++] = data[i];
			delete[] data;
			data = dataf;
			nRowsRead = n_good_rows;
		} else {
			delete[] data;
			data = 0;
			nRowsRead = 0;
		}
	}
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
	if(applyFilter) {
		if(GetFilteredRows(frow, nelem)) {
			if(n_good_rows == 0) {
				nRowsRead = 0;
				return 0;
			}
		} else {
			nRowsRead = 0;
			return 0;
		}
	}
	uint16_t* data = new uint16_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);
	if (status) {
		delete [] data;
		nRowsRead = 0;
		char* errms; std::string msg("Error in InputFileFITS::Read_TUSHORT() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		return 0;
	}
	nRowsRead = lrow - frow + 1;
	if(applyFilter) {
		if(n_good_rows > 0) {
			uint16_t* dataf = (uint16_t*) new uint16_t[n_good_rows];
			long j = 0;
			for(long i=0; i<nelem; i++)
				if(row_status[i] == 1)
					dataf[j++] = data[i];
			delete[] data;
			data = dataf;
			nRowsRead = n_good_rows;
		} else {
			delete[] data;
			data = 0;
			nRowsRead = 0;
		}
	}
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
	if(applyFilter) {
		if(GetFilteredRows(frow, nelem)) {
			if(n_good_rows == 0) {
				nRowsRead = 0;
				return 0;
			}
		} else {
			nRowsRead = 0;
			return 0;
		}
	}
	uint32_t* data = new uint32_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);
	if (status) {
		delete [] data;
		nRowsRead = 0;
		char* errms; std::string msg("Error in InputFileFITS::Read_TUINT() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		return 0;
	}
	nRowsRead = lrow - frow + 1;
	if(applyFilter) {
		if(n_good_rows > 0) {
			uint32_t* dataf = (uint32_t*) new uint32_t[n_good_rows];
			long j = 0;
			for(long i=0; i<nelem; i++)
				if(row_status[i] == 1)
					dataf[j++] = data[i];
			delete[] data;
			data = dataf;
			nRowsRead = n_good_rows;
		} else {
			delete[] data;
			data = 0;
			nRowsRead = 0;
		}
	}
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
	if(applyFilter) {
		if(GetFilteredRows(frow, nelem)) {
			if(n_good_rows == 0) {
				nRowsRead = 0;
				return 0;
			}
		} else {
			nRowsRead = 0;
			return 0;
		}
	}
	uint64_t* data = (uint64_t*) new uint64_t[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);
	if (status) {
		delete [] data;
		nRowsRead = 0;
		char* errms; std::string msg("Error in InputFileFITS::Read_TULONG() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		return 0;
	}
	nRowsRead = lrow - frow + 1;
	if(applyFilter) {
		if(n_good_rows > 0) {
			uint64_t* dataf = (uint64_t*) new uint64_t[n_good_rows];
			long j = 0;
			for(long i=0; i<nelem; i++)
				if(row_status[i] == 1)
					dataf[j++] = data[i];
			delete[] data;
			data = dataf;
			nRowsRead = n_good_rows;
		} else {
			delete[] data;
			data = 0;
			nRowsRead = 0;
		}
	}
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
	if(applyFilter) {
		if(GetFilteredRows(frow, nelem)) {
			if(n_good_rows == 0) {
				nRowsRead = 0;
				return 0;
			}
		} else {
			nRowsRead = 0;
			return 0;
		}
	}
	float* data = new float[nelem];
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);
	if (status) {
		delete [] data;
		nRowsRead = 0;
		char* errms; std::string msg("Error in InputFileFITS::Read_TFLOAT() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		return 0;
	}
	nRowsRead = lrow - frow + 1;
	if(applyFilter) {
		if(n_good_rows > 0) {
			float* dataf = (float*) new float[n_good_rows];
			long j = 0;
			for(long i=0; i<nelem; i++)
				if(row_status[i] == 1)
					dataf[j++] = data[i];
			delete[] data;
			data = dataf;
			nRowsRead = n_good_rows;
		} else {
			delete[] data;
			data = 0;
			nRowsRead = 0;
		}
	}
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
	if(applyFilter) {
		//cout << "(1) ncol " << ncol << " frow " << frow << " lrow " << lrow << " nelem " << nelem << endl;
		if(GetFilteredRows(frow, nelem)) {
			if(n_good_rows == 0) {
				nRowsRead = 0;
				return 0;
			}
		} else {
			nRowsRead = 0;
			return 0;
		}
	}
	double* data = (double*) new double[nelem];
	//cout << "(2) ncol " << ncol << " frow " << frow << " nelem " << nelem << endl;
	fits_read_col(infptr, typecode, ncol + GetIndexFirstColumn(), frow, felem, nelem, &null,  data, &anynull, &status);
	if (status) {
		delete [] data;
		nRowsRead = 0;
		char* errms; std::string msg("Error in InputFileFITS::Read_TDOUBLE() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		return 0;
	}
	nRowsRead = lrow - frow + 1;
	if(applyFilter) {
		if(n_good_rows > 0) {
			double* dataf = (double*) new double[n_good_rows];
			long j = 0;
			for(long i=0; i<nelem; i++)
				if(row_status[i] == 1)
					dataf[j++] = data[i];
			delete[] data;
			data = dataf;
			nRowsRead = n_good_rows;
		} else {
			delete[] data;
			data = 0;
			nRowsRead = 0;
		}
	}
	return data;
}

void InputFileFITS::SetFilter(InputFileFilter* filter) {
	this->filter = filter;
}

bool InputFileFITS::GetFilteredRows(int64_t frow, int64_t nrows) {
	status = 0;
	if(nrows > row_status_size) {
		delete[] row_status;
		row_status = (char*) new char[nrows];
		row_status_size = nrows;
	}
	n_good_rows = 0;
	//cout << "(2) GetFilteredRows() frow " << frow << " nrows " << nrows << endl;
	//fits_find_rows(infptr, (char*)selectEvent.Data(), frow, nrows, &n_good_rows, row_status, &status );
	if(!filter->Open(filename))
		return kFALSE;
	if(!filter->Calculate(frow, nrows))
		return kFALSE;
	if(!filter->Close())
		return kFALSE;
	n_good_rows = filter->GetNGoodRows();
	row_status = filter->GetRowStatus();
	//cout << "(2) GetFilteredRows() frow " << frow << " nrows " << nrows << " n_good_rows " << n_good_rows << " status " << status << endl;
	if (status) {
		char* errms; std::string msg("Error in InputFileFITS::GetFilteredRows() ");
		fits_read_errmsg(errms);
		msg += 	errms;
// 		gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		status = 0;
		return kFALSE;
	}
	return kTRUE;
}

}
