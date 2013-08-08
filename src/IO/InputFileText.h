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

#ifndef QL_IO_INPUTFILETEXT_H
#define QL_IO_INPUTFILETEXT_H

#include <fstream>
#include <stdint.h>
#include <vector>
#include "InputFile.h"

namespace qlbase {

class InputFileText : public InputFile {

	public:

		InputFileText(const std::string &separator = std::string(" "));

		virtual ~InputFileText();

		virtual void open(const std::string &filename);
		virtual void close();
		virtual bool isOpened(){ return opened; }

		void jumpToChunk(int number) {}

		virtual int32_t getNCols(){ return ncols; }
		virtual int64_t getNRows(){ return nrows; }

		virtual std::vector<uint8_t> readu8i(int ncol, long frow, long lrow);
		virtual std::vector<int16_t> read16i(int ncol, long frow, long lrow);
		virtual std::vector<int32_t> read32i(int ncol, long frow, long lrow);
		virtual std::vector<int64_t> read64i(int ncol, long frow, long lrow);
		virtual std::vector<float> read32f(int ncol, long frow, long lrow);
		virtual std::vector<double> read64f(int ncol, long frow, long lrow);

		virtual Image<uint8_t> readImageu8i(){ throw IOException("readImageu8i not supported", 0); }
		virtual Image<int16_t> readImage16i(){ throw IOException("readImageu8i not supported", 0); }
		virtual Image<int32_t> readImage32if(){ throw IOException("readImageu8i not supported", 0); }
		virtual Image<int64_t> readImage64i(){ throw IOException("readImageu8i not supported", 0); }
		virtual Image<float> readImage32f(){ throw IOException("readImageu8i not supported", 0); }
		virtual Image<double> readImage64f(){ throw IOException("readImageu8i not supported", 0); }

	private:
		bool opened;

		std::ifstream fileStream;
		std::string separator;

		void pointTo(int line);
		bool findField(std::string& line, int& first, int& last, int pos = 0);
		bool reopen();
		bool test(int ncol, long frow, long& lrow);

		int nrows, ncols;

		template<class T>
		void readData(std::vector<T> &buff, int ncol, long frow, long lrow);

		void _printState();
};

}

#endif
