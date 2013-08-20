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

#ifndef QL_IO_FILE_H
#define QL_IO_FILE_H

#include <string>
#include <stdexcept>

namespace qlbase {

class IOException : public std::runtime_error {

    int _errorCode;

	public:

	IOException(const std::string &msg, int errorCode = 0) : std::runtime_error(msg), _errorCode(errorCode) {}

	int getErrorCode() {
		return _errorCode;
	}
};

/** The interface for a generic file divided into chunks.
 */
class File {

public:

	File() : _filename("") {}
	virtual ~File() {}

	virtual void open(const std::string &filename) {
		_filename = filename;
	}
	virtual void close() = 0;
	virtual bool isOpened() = 0;

	virtual std::string getFileName() { return _filename; };

	virtual void moveToHeader(int number) = 0;

protected:

	std::string _filename;
};

}

#endif
