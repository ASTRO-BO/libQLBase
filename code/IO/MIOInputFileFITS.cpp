#include "InputFileFITS.h"
#include <TArrayS.h>
#include <TArrayD.h>
#include <TArrayF.h>
#include <TArrayI.h>
#include <TArrayL.h>
#include <iostream>
// #include "QLGlobalMemory.h"
#include "File.h"
using namespace std;
#define IFFMAXROWSTATUS 1000000

InputFileFITS::InputFileFITS() : InputFile() {
	row_status = 0;
	row_status = (char*) new char[IFFMAXROWSTATUS];
	row_status_size = IFFMAXROWSTATUS;
}


InputFileFITS::~InputFileFITS() {
	delete[] row_status;
}

Bool_t InputFileFITS::Open(const std::string &filename) {
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

Int_t InputFileFITS::GetNCols() {
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

Long_t InputFileFITS::GetNRows() {
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
Bool_t InputFileFITS::Close() {
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


Bool_t InputFileFITS::MoveHeader(int header_number) {
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

Bool_t InputFileFITS::ApplyFilter(const std::string &selectEvent) {
	applyFilter = kTRUE;
	this->selectEvent = selectEvent;
}

void InputFileFITS::RemoveFilter() {
	applyFilter = kFALSE;
}


Long_t InputFileFITS::GetNextRowPeriod(UInt_t timeColumnNumber, Long_t pos_first, DOUBLE_T end_time) {
	DOUBLE_T* first_time = READ_TDOUBLE(timeColumnNumber, pos_first, pos_first);
	if(first_time == 0) return 0;
	//DOUBLE_T end_time = (*first_time) + deltaT;
	DOUBLE_T current_time = *first_time;
	Long_t next_pos = pos_first;
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


DOUBLE_T InputFileFITS::GetTime(UInt_t timeColumnNumber, ULong_t pos) {
	MoveHeader(headerBase);
	DOUBLE_T* new_time = READ_TDOUBLE(timeColumnNumber , pos+ GetIndexFirstRow(), pos+ GetIndexFirstRow());
	DOUBLE_T ret = *new_time;
	delete[] new_time;
	return ret;
}

DOUBLE_T* InputFileFITS::GetTime(UInt_t timeColumnNumber, ULong_t start, ULong_t dim) {
	MoveHeader(headerBase);
	DOUBLE_T* ret = READ_TDOUBLE(timeColumnNumber , start+ GetIndexFirstRow(), start+dim-1+ GetIndexFirstRow());
	return ret;
}

UChar_t* InputFileFITS::Read_TBYTE(int ncol, long frow, long lrow, Long_t nelements) {
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
	UChar_t* data = (UChar_t*) new UChar_t[nelem];
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
			UChar_t* dataf = (UChar_t*) new UChar_t[n_good_rows];
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


Short_t* InputFileFITS::Read_TSHORT(int ncol, long frow, long lrow, Long_t nelements) {
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
	Short_t* data = (Short_t*) new Short_t[nelem];
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
			Short_t* dataf = (Short_t*) new Short_t[n_good_rows];
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


Int_t* InputFileFITS::Read_TINT(int ncol, long frow, long lrow, Long_t nelements) {
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
	Int_t* data = new Int_t[nelem];
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
			Int_t* dataf = (Int_t*) new Int_t[n_good_rows];
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


Long_t* InputFileFITS::Read_TINT32BIT(int ncol, long frow, long lrow, Long_t nelements) {
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
	Long_t* data = new Long_t[nelem];
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
			Long_t* dataf = (Long_t*) new Long_t[n_good_rows];
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


UShort_t* InputFileFITS::Read_TUSHORT(int ncol, long frow, long lrow, Long_t nelements) {
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
	UShort_t* data = new UShort_t[nelem];
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
			UShort_t* dataf = (UShort_t*) new UShort_t[n_good_rows];
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


UInt_t* InputFileFITS::Read_TUINT(int ncol, long frow, long lrow, Long_t nelements) {
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
	UInt_t* data = new UInt_t[nelem];
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
			UInt_t* dataf = (UInt_t*) new UInt_t[n_good_rows];
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


ULong_t* InputFileFITS::Read_TULONG(int ncol, long frow, long lrow, Long_t nelements) {
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
	ULong_t* data = (ULong_t*) new ULong_t[nelem];
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
			ULong_t* dataf = (ULong_t*) new ULong_t[n_good_rows];
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


Float_t* InputFileFITS::Read_TFLOAT(int ncol, long frow, long lrow, Long_t nelements) {
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
	Float_t* data = new Float_t[nelem];
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
			Float_t* dataf = (Float_t*) new Float_t[n_good_rows];
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


Double_t* InputFileFITS::Read_TDOUBLE(int ncol, long frow, long lrow, Long_t nelements) {
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
	Double_t* data = (Double_t*) new Double_t[nelem];
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
			Double_t* dataf = (Double_t*) new Double_t[n_good_rows];
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

Bool_t InputFileFITS::GetFilteredRows(Long_t frow, Long_t nrows) {
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
