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

		virtual int getNCols();
		virtual long getNRows();

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
