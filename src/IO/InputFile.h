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

#ifndef QL_IO_INPUTFILE_H
#define QL_IO_INPUTFILE_H

#include <string>
#include <stdint.h>
#include <vector>
#include "File.h"

namespace qlbase {

template<class T>
struct Image {
	std::vector<T> data;
	int32_t dim;
	std::vector<int64_t> sizes;
};

/// The interface for reading from a generic file divided into chunks.
/// It specialize File adding functions for reading tables and images.
class InputFile : public File {

	public:

		InputFile() {}

		virtual ~InputFile() {}

		/* Table chunk functions */
		virtual int getNCols() = 0;
		virtual long getNRows() = 0;

		virtual std::vector<uint8_t> readu8i(int ncol, long frow, long lrow) = 0;
		virtual std::vector<int16_t> read16i(int ncol, long frow, long lrow) = 0;
		virtual std::vector<int32_t> read32i(int ncol, long frow, long lrow) = 0;
		virtual std::vector<int64_t> read64i(int ncol, long frow, long lrow) = 0;
		virtual std::vector<float> read32f(int ncol, long frow, long lrow) = 0;
		virtual std::vector<double> read64f(int ncol, long frow, long lrow) = 0;

		virtual std::vector< std::vector<uint8_t> > readu8iv(int ncol, long frow, long lrow, int vsize) = 0;
		virtual std::vector< std::vector<int16_t> > read16iv(int ncol, long frow, long lrow, int vsize) = 0;
		virtual std::vector< std::vector<int32_t> > read32iv(int ncol, long frow, long lrow, int vsize) = 0;
		virtual std::vector< std::vector<int64_t> > read64iv(int ncol, long frow, long lrow, int vsize) = 0;
		virtual std::vector< std::vector<float> > read32fv(int ncol, long frow, long lrow, int vsize) = 0;
		virtual std::vector< std::vector<double> > read64fv(int ncol, long frow, long lrow, int vsize) = 0;

		virtual Image<uint8_t> readImageu8i() = 0;
		virtual Image<int16_t> readImage16i() = 0;
		virtual Image<int32_t> readImage32if() = 0;
		virtual Image<int64_t> readImage64i() = 0;
		virtual Image<float> readImage32f() = 0;
		virtual Image<double> readImage64f() = 0;
};

}

#endif
