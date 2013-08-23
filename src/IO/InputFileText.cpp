/***************************************************************************
    begin                : Jul 18 2013
    copyright            : (C) 2013 Andrea Zoli
    email                : zoli@iasfbo.inaf.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software for non commercial purpose              *
 *   and for public research institutes; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License.          *
 *   For commercial purpose see appropriate license terms                  *
 *                                                                         *
 ***************************************************************************/

#include <string>
#include <fstream>
#include <sstream>
#include "Definitions.h"
#include "InputFileText.h"

namespace qlbase
{

template<class T>
void InputFileText::readData(std::vector<T> &buff, int ncol, long frow, long lrow)
{
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
}

InputFileText::InputFileText(const std::string &separator) : opened(false), nrows(0), ncols(0) {
	this->separator = separator;
}

InputFileText::~InputFileText() {
}

void InputFileText::open(const std::string &filename) {

	// Close prev input file stream
	close();

	fileStream.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

	// Try to open new file
	fileStream.open(filename.c_str());

	// Read first line
	std::string line;
	if(!getline(fileStream,line)) {
		close();
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

	opened = true;
}

void InputFileText::close() {
	nrows    = 0;
	ncols    = 0;

	opened = false;
}

bool InputFileText::findField(std::string& line, int& first, int& last, int pos) {

	if(line.length()==0)
		return false;

	first = line.find_first_not_of(separator,pos);
	if(first<0)
		return false;

	last = line.find_first_of(separator,first);

	return true;
}

bool InputFileText::reopen() {
	if(fileStream) {
		if(!fileStream.good()) {
			close();
			fileStream.open(_filename.c_str());
		}
		else {
			fileStream.seekg(0);
		}
		return true;
	}
	return false;
}

bool InputFileText::test(int ncol, long frow, long& lrow) {
	if(ncol>=ncols)
		return false;
	if(frow < 0 || frow > lrow)
		return false;
	// Calculates last row index
	if(lrow>nrows - 1) {
		lrow = nrows - 1;
	}
	return true;
}

std::vector<uint8_t> InputFileText::readu8i(int ncol, long frow, long lrow) {
	std::vector<uint8_t> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

std::vector<int16_t> InputFileText::read16i(int ncol, long frow, long lrow) {
	std::vector<int16_t> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

std::vector<int32_t> InputFileText::read32i(int ncol, long frow, long lrow) {
	std::vector<int32_t> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

std::vector<int64_t> InputFileText::read64i(int ncol, long frow, long lrow) {
	std::vector<int64_t> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

std::vector<float> InputFileText::read32f(int ncol, long frow, long lrow) {
	std::vector<float> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

std::vector<double> InputFileText::read64f(int ncol, long frow, long lrow) {
	std::vector<double> buff;
	readData(buff, ncol, frow, lrow);
	return buff;
}

void InputFileText::_printState() {
	if(fileStream) {
		DEBUG("File: " << _filename << "(" << fileStream.rdstate() << ") ");
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
