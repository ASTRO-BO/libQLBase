/***************************************************************************
						  File.h  -  description
							 -------------------
	begin                : Thu Nov 29 2001
	copyright            : (C) 2001 by Andrea Bulgarelli
	email                : bulgarelli@tesre.bo.cnr.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef QL_IO_FILE_H
#define QL_IO_FILE_H

#include <string>
#include <cstdint>
#include "QLDefinition.h"

namespace ql_io {

//##ModelId=3C0F6C190309
//##Documentation
//##	$Date: 2006/09/12 13:36:24 $
//##	$Header: /home/repository/cvs/gtb/libQL2/code/File.h,v 1.1 2006/09/12 13:36:24 bulgarelli Exp $
//##	$Id: File.h,v 1.1 2006/09/12 13:36:24 bulgarelli Exp $
//##	$Revision: 1.1 $
//##	\brief This class represent a device File for IO system.


class File {
	public:

		//##ModelId=3C187750028F
		//##Documentation
		//## \param b Indicates if the machine is big or little endian. If little endian, this class
		//## swap the byte of a word
		//## \param startP Starting position whitin File
		File(long startP = 0);

		virtual ~File() {};

		//##ModelId=3C0F6C1A0013
		//##Documentation
		//## Open the File
		//## \param Filename Name of File
		//## \param mode Mode of File opened. r for read, w for writing
		virtual bool Open(const std::string &filename, char* mode="r");

		//##ModelId=3C0F6C1A0016
		//## Documentation
		//## Get a single byte from current position of File opened.
		//## \pre The File must be opened.
		virtual int GetByte();

		//##ModelId=3C0F6C1A001B
		//##Documentation
		//## Get the current line.
		//## \pre The File must be opened.
		virtual std::string GetLine();

		//##ModelId=3C0F6C1A001B
		//##Documentation
		//## Get the first configuration line (a line that don't start with --)
		//## \pre The File must be opened.
		virtual const std::string GetConfigurationLine();

		virtual const std::string GetLine(const std::string &s);

		//##ModelId=3C0F6C1A001F
		//##Documentation
		//## Close the File.
		virtual void Close( );

		//##ModelId=3C0F6C1A0021
		//##Documentation
		//## \return true if File is closed, false if File is opened.
		virtual bool IsClosed();

		//##ModelId=3C15F42303C4
		//##Documentation
		//## \return Returns last line read from File.
		std::string GetLastLineRead();

		//##ModelId=3C205AF20278
		//## Sets the position into File.
		//## \pre The File must be opened.
		virtual long Setpos(long offset);

		//##ModelId=3C205AF202C3
		//## Gets the current File position.
		//## \pre The File must be opened.
		virtual long Getpos();

		//##ModelId=3C205AF20313
		//##Documentation
		//## Stores the current position
		//## \pre The File must be opened.
		virtual bool MemBookmarkPos();

		//##ModelId=3C205AF20334
		//##Documentation
		//## Sets the position stored with memBookmarkPos() method.
		//## \pre The File must be opened.
		virtual bool SetLastBookmarkPos();

		//##ModelId=3A5A30B40368
		//##Documentation
		//## \return Returns true is the position into File is at end of File
		virtual bool IsEOF();

		//##ModelId=3C51324A022E
		//## \pre The File must be opened.
		int SetFirstPos();

		//##ModelId=3C51324A0299
		//## Changes directory. The path is in the Filename of File opened.
		//## \pre The File name must be setted.
		bool Fchdir();

		//##ModelId=3C51324A0304
		long Find(uint8_t b);

		//##ModelId=3EADC12800D8
		//##Documentation
		//## Writes a string into opend File.
		//## \pre The File must be opened in w mode
		bool WriteString(const std::string &str);

		bool WriteStringWithEndl(const std::string &str);

		//##Documentation
		//## Write an array of string into a file. If a filename is specified,
		//## open the file in rw mode, write the array and close the file.
		bool WriteStringArray(const char* array[], const std::string &filename = "");

		void DeleteFile(const std::string &filename);

		//##ModelId=3EADC12702C9
		static unsigned long byte_read;

		//##ModelId=3EADC1270363
		static unsigned long char_read;

	protected:

		//##ModelId=3C8774400282
		bool eof;

		//##ModelId=3C8774400296
		bool closed;

		//##ModelId=3C8774400297
		bool bigendian;

		//##ModelId=3ACCD75E038A
		std::string lastLineRead;

		//##ModelId=3DA3E56B0064
		FILE* fp;

		//##ModelId=3C187750023E
		long bookmarkPos;

		//##ModelId=3C51324900FB
		long startPosition;

		//##ModelId=3C5132490213
		std::string filename;
};

}

#endif
