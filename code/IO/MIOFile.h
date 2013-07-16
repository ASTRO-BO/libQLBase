/***************************************************************************
						  MIOFile.h  -  description
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
#ifndef MIOFile_h_h
#define MIOFile_h_h
#include <TObject.h>
#include <TString.h>
#include "QLDefinition.h"

//##ModelId=3C0F6C190309
//##Documentation
//##	$Date: 2006/09/12 13:36:24 $
//##	$Header: /home/repository/cvs/gtb/libQL2/code/MIOFile.h,v 1.1 2006/09/12 13:36:24 bulgarelli Exp $
//##	$Id: MIOFile.h,v 1.1 2006/09/12 13:36:24 bulgarelli Exp $
//##	$Revision: 1.1 $
//##	\brief This class represent a device MIOFile for IO system.


class MIOFile {
	public:

		//##ModelId=3C187750028F
		//##Documentation
		//## \param b Indicates if the machine is big or little endian. If little endian, this class
		//## swap the byte of a word
		//## \param startP Starting position whitin MIOFile
		MIOFile(long startP = 0);

		virtual ~MIOFile() {};

		//##ModelId=3C0F6C1A0013
		//##Documentation
		//## Open the MIOFile
		//## \param MIOFilename Name of MIOFile
		//## \param mode Mode of MIOFile opened. r for read, w for writing
		virtual bool Open( TString filename, char* mode="r") ;

		//##ModelId=3C0F6C1A0016
		//## Documentation
		//## Get a single byte from current position of MIOFile opened.
		//## \pre The MIOFile must be opened.
		virtual int GetByte();

		//##ModelId=3C0F6C1A001B
		//##Documentation
		//## Get the current line.
		//## \pre The MIOFile must be opened.
		virtual TString GetLine();

		//##ModelId=3C0F6C1A001B
		//##Documentation
		//## Get the first configuration line (a line that don't start with --)
		//## \pre The MIOFile must be opened.
		virtual TString GetConfigurationLine();

		virtual TString GetLine(TString s);

		//##ModelId=3C0F6C1A001F
		//##Documentation
		//## Close the MIOFile.
		virtual void Close( );

		//##ModelId=3C0F6C1A0021
		//##Documentation
		//## \return true if MIOFile is closed, false if MIOFile is opened.
		virtual bool IsClosed();

		//##ModelId=3C15F42303C4
		//##Documentation
		//## \return Returns last line read from MIOFile.
		TString GetLastLineRead();

		//##ModelId=3C205AF20278
		//## Sets the position into MIOFile.
		//## \pre The MIOFile must be opened.
		virtual long Setpos(long offset);

		//##ModelId=3C205AF202C3
		//## Gets the current MIOFile position.
		//## \pre The MIOFile must be opened.
		virtual long Getpos();

		//##ModelId=3C205AF20313
		//##Documentation
		//## Stores the current position
		//## \pre The MIOFile must be opened.
		virtual bool MemBookmarkPos();

		//##ModelId=3C205AF20334
		//##Documentation
		//## Sets the position stored with memBookmarkPos() method.
		//## \pre The MIOFile must be opened.
		virtual bool SetLastBookmarkPos();

		//##ModelId=3A5A30B40368
		//##Documentation
		//## \return Returns true is the position into MIOFile is at end of MIOFile
		virtual bool IsEOF();

		//##ModelId=3C51324A022E
		//## \pre The MIOFile must be opened.
		int SetFirstPos();

		//##ModelId=3C51324A0299
		//## Changes directory. The path is in the MIOFilename of MIOFile opened.
		//## \pre The MIOFile name must be setted.
		bool Fchdir();

		//##ModelId=3C51324A0304
		long Find(UChar_t b);

		//##ModelId=3EADC12800D8
		//##Documentation
		//## Writes a string into opend MIOFile.
		//## \pre The MIOFile must be opened in w mode
		bool WriteString(TString str);
		
		bool WriteStringWithEndl(TString str);
		
		//##Documentation
		//## Write an array of string into a file. If a filename is specified,
		//## open the file in rw mode, write the array and close the file.
		bool WriteStringArray(const char* array[], TString filename = "");
		
		void DeleteFile(TString filename);

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
		TString lastLineRead;

		//##ModelId=3DA3E56B0064
		FILE* fp;

		//##ModelId=3C187750023E
		long bookmarkPos;

		//##ModelId=3C51324900FB
		long startPosition;

		//##ModelId=3C5132490213
		TString filename;
};


#endif
