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
#include <cstdint>

namespace ql_io {

/** This class represent a device File for IO system. */
class File {
	public:

		File(long startP = 0);

		virtual ~File() {};

		/** Open the File.
		 * @param filename Name of the file.
		 * @param mode Mode of File opened. r for read, w for writing.
		 */
		virtual bool Open(const std::string &filename, char* mode="r");

		/** Get a single byte from current position of File opened.
		 * @pre The File must be opened.
		 */
		virtual int GetByte();

		/** Get the current line.
		 * @pre The File must be opened.
		 */
		virtual std::string GetLine();

		/** Get the first configuration line (a line that don't start with --).
		 * @pre The File must be opened.
		*/
		virtual const std::string GetConfigurationLine();

		virtual const std::string GetLine(const std::string &s);

		virtual void Close( );

		virtual bool IsClosed();

		std::string GetLastLineRead();

        /** Sets the position into File.
		 * @pre The File must be opened.
         */
		virtual long Setpos(long offset);

		/** Gets the current File position.
		 * @pre The File must be opened.
         */
		virtual long Getpos();

		/** Stores the current position
		 * @pre The File must be opened.
         */
		virtual bool MemBookmarkPos();

		/* Sets the position stored with memBookmarkPos() method.
		 * @pre The File must be opened.
		 */
		virtual bool SetLastBookmarkPos();

		virtual bool IsEOF();

		int SetFirstPos();

		/**Changes directory. The path is in the Filename of File opened.
		 * @pre The File name must be setted.
         */
		bool Fchdir();

		long Find(uint8_t b);

		/** Writes a string into opend File.
		 * @pre The File must be opened in w mode.
         */
		bool WriteString(const std::string &str);

		bool WriteStringWithEndl(const std::string &str);

		/** Write an array of string into a file. If a filename is specified,
		 * the file in rw mode, write the array and close the file.
         */
		bool WriteStringArray(const char* array[], const std::string &filename = "");

		void DeleteFile(const std::string &filename);

		static unsigned long byte_read;

		static unsigned long char_read;

	protected:

		bool eof;

		bool closed;

		bool bigendian;

		std::string lastLineRead;

		FILE* fp;

		long bookmarkPos;

		long startPosition;

		std::string filename;
};

}

#endif
