//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "MIOInputFileFilter.h"
// #include "QLGlobalMemory.h"


MIOInputFileFilter::MIOInputFileFilter()
{
	row_status = 0;
	n_good_rows = 0;
}


MIOInputFileFilter::~MIOInputFileFilter()
{
}

Bool_t MIOInputFileFilter::Open(TString filename) {
	status = 0;
	this->filename = filename;
	if ( !fits_open_table(&infptr, filename, READONLY, &status) ) {
		opened = true;
	}
	else
		opened = kFALSE;

	if (status) {
		char* errms; TString msg = "Error in MIOInputFileFilter::Open() ";
		fits_read_errmsg(errms);
		msg += 	errms;
		//gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		status = 0;
		return kFALSE;
	}
	return kTRUE;
}

Bool_t MIOInputFileFilter::Close() {
	status = 0;
	fits_close_file(infptr, &status);
	if (status) {
		char* errms; TString msg = "Error in MIOInputFileFilter::Close() ";
		fits_read_errmsg(errms); 
		msg += errms;
		//gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		opened = kTRUE;
		status = 0;
		return kFALSE;
	}
	opened = kFALSE;
	return kTRUE;
}

Bool_t MIOInputFileFilter::MoveHeader(int header_number) {
	status = 0;
	fits_movabs_hdu(infptr, header_number + 1, 0, &status);
	if (status) {
		char* errms; TString msg = "Error in MIOInputFileFilter::MoveHeader() ";
		//fits_read_errmsg(errms);
		//msg += errms;	
		//gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		status = 0;
		return false;
	}
	return true;
}

int MIOInputFileFilter::GetColNum(char* nomecol) {
	int colnum = -1;
	fits_get_colnum(infptr,CASEINSEN,nomecol,&colnum,&status);	
	if (status) {
		char* errms; TString msg = "Error in MIOInputFileFilter::GetColNum() ";
		fits_read_errmsg(errms);
		msg += errms;	
		//gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;
		fits_report_error(stderr, status);
		status = 0;
		return false;
	}	
	return colnum;
}


DOUBLE_T* MIOInputFileFilter::ReadCol(int headernum, char* colname, Long_t frow, Long_t nrows) {
	
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
	DOUBLE_T null = 0.0;
	
	delete[] row_status;
	row_status = (char*) new char[nrows];
	int colnum;
	bool res= MoveHeader(headernum);
	if(res == false)
		return 0;
	colnum = GetColNum(colname);
	if(colnum == -1)
		return 0;
	DOUBLE_T* data = (DOUBLE_T*) new DOUBLE_T[nrows];	
	fits_read_col(infptr, typecode, colnum, frow, felem, nrows, &null,  data, &anynull, &status);
	if (status) {
		char* errms; TString msg = "Error in MIOInputFileFilter::ReadCol(): ";
		//fits_read_errmsg(errms); 
		//msg += errms;
		//gm.PrintLogMessage(msg, kTRUE);
		cerr << msg << endl;		
		fits_report_error(stderr, status);
		status = 0;
		return false;
	}	
	return data;
}

