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
#include <fstream>
#include <sstream>
#include "Definitions.h"
#include "InputTextFile.h"

namespace qlbase
{

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
                        std::string line;                                                  \
                        if(getline(fileStream,line)) {                               \
                          if(line.size()==0) {                                        \
                            i--;                                                      \
                            continue;                                                 \
                          }                                                           \
                          if(i>=frow) {                                               \
                            int first = 0;                                            \
                            int last  = 0;                                            \
                            for(int j = 0; j <= ncol; j++)                            \
                              findField(line,first,last,last);                        \
                            std::istringstream ist(std::string(line,first,last-first));         \
                            ist >> buff[buff_off++];                                  \
                          }                                                           \
                        }                                                             \
                      }                                                               \
                      nRowsRead = buff_sz;                                            \
                      return buff;

InputTextFile::InputTextFile(const std::string &separator) {
	this->separator = separator;
	status = kFileNoError;
}

InputTextFile::~InputTextFile() {
}

/*double* InputTextFile::GetTime(uint32_t timeColumnNumber, uint64_t start, uint64_t dim) {
	double* ret = read64f(timeColumnNumber , start+ GetIndexFirstRow(), start+dim-1+ GetIndexFirstRow());
	return ret;
}
*/
bool InputTextFile::Open(const std::string &filename) {

	// Close prev input file stream
	Close();

	// Try to open new file
	fileStream.open(filename.c_str());
	if(!fileStream.is_open()) {
		opened = false;
		return false;
	}

	// Save file name
	opened = true;
	this->filename = filename;

	// Read first line
	std::string line;
	if(!getline(fileStream,line)) {
		Close();
		return false;
	} else
		nrows++;

	// Count cols
	int first = 0;
	int last  = 0;
	while(findField(line,first,last,last))
		ncols++;

	// Count rows
	while(getline(fileStream,line))
		if(line.size())
			nrows++;

	return true;
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
	nrows    = 0;
	ncols    = 0;
	filename = "";
	opened   = false;
	status   = kFileNoError;
	return true;
}

bool InputTextFile::reopen() {
	if(fileStream) {
		if(!fileStream.good()) {
			Close();
			fileStream.open(filename.c_str());
		}
		else {
			fileStream.seekg(0);
		}
		return true;
	}
	return false;
}

bool InputTextFile::test(int ncol, long frow, long& lrow) {
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
		return true;
	return false;
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
		std::string line;
		if(getline(fileStream,line)) {

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
				std::istringstream ist(std::string(line,first,last-first));
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
		DEBUG("File: " << filename << "(" << fileStream.rdstate() << ") ");
		if(fileStream.rdstate()&std::ifstream::badbit)
			DEBUG("Error: critical error in stream buffer");
		if(fileStream.rdstate()&std::ifstream::eofbit)
			DEBUG("Error: End-Of-File reached while extracting");
		if(fileStream.rdstate()&std::ifstream::failbit)
			DEBUG("Error: failure extracting from stream");
		if(fileStream.rdstate()==0)
			DEBUG("no errors\n");
	}
	else
		DEBUG("File: closed\n");

	DEBUG(" row: " << nrows)
	DEBUG(" col: " << ncols);
}

}
