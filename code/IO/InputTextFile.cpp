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

#include <string>
#include <iostream>
#include <sstream>
#include "InputTextFile.h"

namespace ql_io
{

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

double* InputTextFile::GetTime(uint32_t timeColumnNumber, uint64_t start, uint64_t dim) {
	double* ret = READ_TDOUBLE(timeColumnNumber , start+ GetIndexFirstRow(), start+dim-1+ GetIndexFirstRow());
	return ret;
}

bool InputTextFile::Open(const std::string &file_name) {

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

bool InputTextFile::Close() {
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

int64_t InputTextFile::GetNextRowPeriod(uint32_t timeColumnNumber, int64_t pos_first, double end_time) {

	return 0;
}

double InputTextFile::GetTime(uint32_t timeColumnNumber, uint64_t pos) {

	return 0;
}

bool InputTextFile::IsOpened() {
	if(fileStream)
		return kTRUE;
	return kFALSE;
}

uint8_t* InputTextFile::Read_TBYTE(int ncol, long frow, long lrow, int64_t nelements) {
	READ_DATA(uint8_t);
}

int16_t* InputTextFile::Read_TSHORT(int ncol, long frow, long lrow, int64_t nelements) {

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
	int16_t*  buff      = new int16_t[buff_sz];

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

int32_t* InputTextFile::Read_TINT(int ncol, long frow, long lrow, int64_t nelements) {
	READ_DATA(int32_t)
}

int64_t* InputTextFile::Read_TINT32BIT(int ncol, long frow, long lrow, int64_t nelements) {
	READ_DATA(int64_t)
}

uint16_t* InputTextFile::Read_TUSHORT(int ncol, long frow, long lrow, int64_t nelements) {
	READ_DATA(uint16_t)
}

uint32_t* InputTextFile::Read_TUINT(int ncol, long frow, long lrow, int64_t nelements) {
	READ_DATA(uint32_t)
}

uint64_t* InputTextFile::Read_TULONG(int ncol, long frow, long lrow, int64_t nelements) {
	READ_DATA(uint64_t)
}

float* InputTextFile::Read_TFLOAT(int ncol, long frow, long lrow, int64_t nelements) {
	READ_DATA(float)
}

double* InputTextFile::Read_TDOUBLE(int ncol, long frow, long lrow, int64_t nelements) {
	READ_DATA(double)
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

}
