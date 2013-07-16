//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QLINPUTFILEFILTER_H
#define QLINPUTFILEFILTER_H
#include "QLDefinition.h"
#include <fitsio.h>
#include <string>
#include <iostream>

using namespace std;
/**
@author bulgarelli
*/

namespace ql_io
{

class InputFileFilter {
public:
	InputFileFilter();

	virtual ~InputFileFilter();

	Bool_t Open(const std::string &filename);

	Bool_t Close();

	virtual Bool_t Calculate(Long_t frow, Long_t nrows) = 0;

	virtual Long_t GetNGoodRows() { return n_good_rows; };

	virtual char* GetRowStatus() { return row_status; };

protected:

	virtual Bool_t MoveHeader(Int_t header_number);

	virtual Int_t GetColNum(char* nomecol);

	virtual DOUBLE_T* ReadCol(Int_t headernum, char* colname, Long_t frow, Long_t nrows);

	fitsfile *infptr;

	Long_t n_good_rows;

	char* row_status;

	Int_t status;

	std::string filename;

	Bool_t opened;

};

}

#endif
