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

template<class T>
void InputTextFile::readData(std::vector<T> &buff, int ncol, long frow, long lrow)
{
/*	if(!test(ncol, frow, lrow)) {
		status = kFileNoRead;
		// FIXME throw something
	}
	if(!reopen()){
		status = kFileNoRead;
		// FIXME throw something return 0;
	}*/
	int buff_sz = lrow - frow + 1;
	int buff_off = 0;

	buff.resize(buff_sz);
	for(int i = 0; i <= lrow; i++) {
		std::string line;
		if(getline(fileStream, line)) {
			if(line.size()==0) {
				i--;
				continue;
			}
			if(i>=frow) {
				int first = 0;
				int last  = 0;
				for(int j = 0; j <= ncol; j++)
					findField(line,first,last,last);
				std::istringstream ist(std::string(line,first,last-first));
				ist >> buff[buff_off++];
			}
		}
	}
	nRowsRead = buff_sz;
}

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

std::vector<int8_t> InputTextFile::read8i(int ncol, long frow, long lrow, int64_t nelements) {
	std::vector<int8_t> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

std::vector<int16_t> InputTextFile::read16i(int ncol, long frow, long lrow, int64_t nelements) {
	std::vector<int16_t> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

std::vector<int32_t> InputTextFile::read32i(int ncol, long frow, long lrow, int64_t nelements) {
	std::vector<int32_t> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

std::vector<int64_t> InputTextFile::read64i(int ncol, long frow, long lrow, int64_t nelements) {
	std::vector<int64_t> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

std::vector<float> InputTextFile::read32f(int ncol, long frow, long lrow, int64_t nelements) {
	std::vector<float> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

std::vector<double> InputTextFile::read64f(int ncol, long frow, long lrow, int64_t nelements) {
	std::vector<double> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
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
