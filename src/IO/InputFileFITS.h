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

#ifndef QL_IO_INPUTFILEFITS_H
#define QL_IO_INPUTFILEFITS_H

#include <stdint.h>
#include <fitsio.h>
#include "InputFile.h"

namespace qlbase {

class InputFileFITS : public InputFile {

	public:

		InputFileFITS();

		virtual ~InputFileFITS();

		virtual void open(const std::string &filename);
		virtual void close();
		virtual bool isOpened() { return opened; }

		virtual void moveToHeader(int number);

		virtual int32_t getNCols();
		virtual int64_t getNRows();

		virtual std::vector<uint8_t> readu8i(int ncol, long frow, long lrow);
		virtual std::vector<int16_t> read16i(int ncol, long frow, long lrow);
		virtual std::vector<int32_t> read32i(int ncol, long frow, long lrow);
		virtual std::vector<int64_t> read64i(int ncol, long frow, long lrow);
		virtual std::vector<float> read32f(int ncol, long frow, long lrow);
		virtual std::vector<double> read64f(int ncol, long frow, long lrow);

		virtual Image<uint8_t> readImageu8i();
		virtual Image<int16_t> readImage16i();
		virtual Image<int32_t> readImage32if();
		virtual Image<int64_t> readImage64i();
		virtual Image<float> readImage32f();
		virtual Image<double> readImage64f();

	private:

	bool opened;
	fitsfile *infptr;

	void throwException(const char *msg, int status);

	template<class T>
	void _read(int ncol, std::vector<T>& buff, int type, long frow, long lrow);

	template<class T>
	void _readImage(Image<T>& buff, int type);

};

}

#endif
