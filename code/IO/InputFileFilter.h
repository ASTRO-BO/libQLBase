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
#ifndef QL_IO_INPUTFILEFILTER_H
#define QL_IO_INPUTFILEFILTER_H

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

	virtual Bool_t Calculate(int64_t frow, int64_t nrows) = 0;

	virtual int64_t GetNGoodRows() { return n_good_rows; };

	virtual char* GetRowStatus() { return row_status; };

protected:

	virtual Bool_t MoveHeader(int32_t header_number);

	virtual int32_t GetColNum(char* nomecol);

	virtual DOUBLE_T* ReadCol(int32_t headernum, char* colname, int64_t frow, int64_t nrows);

	fitsfile *infptr;

	int64_t n_good_rows;

	char* row_status;

	int32_t status;

	std::string filename;

	Bool_t opened;

};

}

#endif
