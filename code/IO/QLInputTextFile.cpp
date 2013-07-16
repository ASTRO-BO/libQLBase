#include "InputTextFile.h"
#include <string>
#include <iostream>
#include <sstream>
#include "QLGlobalMemory.h"
using namespace std;

#define SAFE_DELETE(p) if(p) { delete p; p = 0; }

#define READ_DATA(ty) if(!test(ncol, frow, lrow)) {                                   \
                        status = kFileNoRead;                                         \
                        return 0;                                                     \
                      }                                                               \
                      if(!reopen()){                                                  \
                        status = kFileNoRead;                                         \
                        return 0;                                                     \
                      }                                                               \
                      int       buff_sz   = lrow - frow + 1;                          \
                      int       buff_off  = 0;                                        \
                      ty*       buff      = new ty[buff_sz];                          \
                      for(int i = 0; i <= lrow; i++) {                                \
                        string line;                                                  \
                        if(getline(*fileStream,line)) {                               \
                          if(line.size()==0) {                                        \
                            i--;                                                      \
                            continue;                                                 \
                          }                                                           \
                          if(i>=frow) {                                               \
                            int first = 0;                                            \
                            int last  = 0;                                            \
                            for(int j = 0; j <= ncol; j++)                            \
                              findField(line,first,last,last);                        \
                            istringstream ist(string(line,first,last-first));         \
                            ist >> buff[buff_off++];                                  \
                          }                                                           \
                        }                                                             \
                      }                                                               \
                      nRowsRead = buff_sz;                                            \
                      return buff;

InputTextFile::InputTextFile(const std::string &separator) : fileStream(0) {
	this->separator = separator;
	status = kFileNoError;
}


InputTextFile::~InputTextFile() {
	SAFE_DELETE(fileStream)
}

DOUBLE_T* InputTextFile::GetTime(UInt_t timeColumnNumber, ULong_t start, ULong_t dim) {
	DOUBLE_T* ret = READ_TDOUBLE(timeColumnNumber , start+ GetIndexFirstRow(), start+dim-1+ GetIndexFirstRow());
	return ret;
}

Bool_t InputTextFile::Open(const std::string &file_name) {

	// Close prev input file stream
	Close();

	// Try to open new file
	fileStream = new ifstream(file_name.Data());
	if(!fileStream->is_open()) {
		SAFE_DELETE(fileStream)
			return kFALSE;
	}

	// Save file name
	opened = kTRUE;
	filename = file_name;

	// Read first line
	string line;
	if(!getline(*fileStream,line)) {
		SAFE_DELETE(fileStream)
			return kFALSE;
	} else
	nrows++;

	// Count cols
	int first = 0;
	int last  = 0;
	while(findField(line,first,last,last))
		ncols++;

	// Count rows
	while(getline(*fileStream,line))
		if(line.size())
			nrows++;

	return kTRUE;
}


bool InputTextFile::findField(std::string& line, int& first, int& last, int pos) {

	if(line.length()==0)
		return false;

	first = line.find_first_not_of(separator,pos);
	if(first<0)
		return false;

	last = line.find_first_of(separator,first);

	return true;
}


Bool_t InputTextFile::Close() {
	SAFE_DELETE(fileStream)
	nrows    = 0;
	ncols    = 0;
	filename = "";
	opened   = kFALSE;
	status   = kFileNoError;
	return kTRUE;
}


bool InputTextFile::reopen() {
	if(fileStream) {
		if(!fileStream->good()) {
			delete fileStream;
			fileStream = new ifstream(filename.Data());
		}
		else {
			fileStream->seekg(0);
		}
		return true;
	}
	return false;
}


bool InputTextFile::test(int ncol, long frow, long& lrow) {
	if(fileStream==0)
		return false;
	if(ncol>=ncols)
		return false;
	if(frow < 0 || frow > lrow)
		return false;
	// Calculates last row index
	if(lrow>nrows - 1) {
		lrow = nrows - 1;
		status = kFileErrorRowsTerminated;
	}
	return true;
}


Long_t InputTextFile::GetNextRowPeriod(UInt_t timeColumnNumber, Long_t pos_first, DOUBLE_T end_time) {

	return 0;
}


DOUBLE_T InputTextFile::GetTime(UInt_t timeColumnNumber, ULong_t pos) {

	return 0;
}


Bool_t InputTextFile::IsOpened() {
	if(fileStream)
		return kTRUE;
	return kFALSE;
}


UChar_t* InputTextFile::Read_TBYTE(int ncol, long frow, long lrow, Long_t nelements) {
	READ_DATA(UChar_t);
}


Short_t* InputTextFile::Read_TSHORT(int ncol, long frow, long lrow, Long_t nelements) {

	// Test inputs
	if(!test(ncol, frow, lrow)) {
		status = kFileNoRead;
		return 0;
	}

	// Reopen file if necessary
	if(!reopen()) {
		status = kFileNoRead;
		return 0;
	}

	// Create buffer
	int       buff_sz   = lrow - frow + 1;
	int       buff_off  = 0;
	Short_t*  buff      = new Short_t[buff_sz];

	// Read rows
	for(int i = 0; i <= lrow; i++) {
		string line;
		if(getline(*fileStream,line)) {

			// Skip withe rows
			if(line.size()==0) {
				i--;
				continue;
			}

			if(i>=frow) {

				// Skip columns
				int first = 0;
				int last  = 0;
				for(int j = 0; j <= ncol; j++)
					findField(line,first,last,last);

				// convert string
				istringstream ist(string(line,first,last-first));
				ist >> buff[buff_off++];
			}
		}
	}
	nRowsRead = buff_sz;
	return buff;
}


Int_t* InputTextFile::Read_TINT(int ncol, long frow, long lrow, Long_t nelements) {
	READ_DATA(Int_t)
}


Long_t* InputTextFile::Read_TINT32BIT(int ncol, long frow, long lrow, Long_t nelements) {
	READ_DATA(Long_t)
}


UShort_t* InputTextFile::Read_TUSHORT(int ncol, long frow, long lrow, Long_t nelements) {
	READ_DATA(UShort_t)
}


UInt_t* InputTextFile::Read_TUINT(int ncol, long frow, long lrow, Long_t nelements) {
	READ_DATA(UInt_t)
}


ULong_t* InputTextFile::Read_TULONG(int ncol, long frow, long lrow, Long_t nelements) {
	READ_DATA(ULong_t)
}


Float_t* InputTextFile::Read_TFLOAT(int ncol, long frow, long lrow, Long_t nelements) {
	READ_DATA(Float_t)
}


Double_t* InputTextFile::Read_TDOUBLE(int ncol, long frow, long lrow, Long_t nelements) {
	READ_DATA(Double_t)
}


void InputTextFile::_printState() {
	if(fileStream) {
		PD("File: " << filename.Data() << "(" << fileStream->rdstate() << ")");
		if(fileStream->rdstate()&ifstream::badbit)
			gm.PrintLogMessage("Error: critical error in stream buffer", kTRUE);
		if(fileStream->rdstate()&ifstream::eofbit)
			gm.PrintLogMessage("Error: End-Of-File reached while extracting", kTRUE);
		if(fileStream->rdstate()&ifstream::failbit)
			gm.PrintLogMessage("Error: failure extracting from stream", kTRUE);
		if(fileStream->rdstate()==0)
			PD(" no error condition\n");
	} else
	PD("File: closed\n");
	PD(" row: " << nrows)
	PD(" col: " << ncols);
}
