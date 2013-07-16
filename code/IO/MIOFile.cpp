/***************************************************************************
						  MIOFile.cpp  -  description
							 -------------------
	begin                : Tue Nov 20 2001
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

#include "MIOFile.h"
#include <unistd.h>

#define EOI -1

//##ModelId=3DA3E56900F0
unsigned long MIOFile::byte_read = 0;
//##ModelId=3DA3E56A0078
unsigned long MIOFile::char_read = 0;



//##ModelId=3C187750028F
MIOFile::MIOFile(long startP) {
//	bigendian = false; //si presume che la macchina sia little endian
	startPosition = startP;
	filename = "";
	closed = true;
}


//##ModelId=3C87744002A2
bool MIOFile::IsClosed() {
	return closed;
}


//##ModelId=3C0F6C1A0013
bool MIOFile::Open(const std::string &filename, char* mode) {
	fp = fopen(filename, mode);

	if(fp == NULL) {
		closed = true;
		eof = true;
		this->filename = "";
		return false;

	}
	else {
		closed = false;
		eof = false;
		SetFirstPos();
		this->filename = filename;
	};
	return !closed;
}


//##ModelId=3C0F6C1A0016
int MIOFile::GetByte() {
	int c;
	if(!closed && !eof) {
		c = fgetc(fp);
		if(c == EOF) {
			eof = true;
			//close();
			c = EOI;
		}
		MIOFile::byte_read++;
		return c;
	}
	else {
		return EOI;
	}
}


//##ModelId=3C0F6C1A001B
std::string MIOFile::GetLine() {
	//char* s = new char[500];
	static char s[512];
	s[0]=0;
	//	static unsigned long dimalloc = 0;
	char c;
	int i = 0;

	if(!closed) {
		c=fgetc(fp);
		MIOFile::char_read++;
		while( c != '\n' && c != EOF ) {
			s[i] = c;
			//printf("%c", c);
			i++;
			if(i>=512)
				return s;
			c=fgetc(fp);
			MIOFile::char_read++;
		}
		if(c == EOF) {
			eof = true;
			//close();
		}
	}
	s[i] = '\0';
	unsigned dims = strlen(s);
	char* sr = new char[ dims + 1];
	for(unsigned i=0; i<=dims; i++)
		sr[i] = s[i];
	lastLineRead = sr;
	return sr;
}


std::string MIOFile::GetConfigurationLine() {
	std::string temp;

	temp = GetLine();
	while(strstr(temp, "--") !=  NULL) {
		temp = GetLine();
	}
	return temp;
}


std::string MIOFile::GetLine(const std::string &s) {
	std::string line;
	line = GetLine();

	while(line.Contains(s)) {
		line = "";
		line = GetLine();
		if(IsEOF() == true)
			break;
	}
	return line;
}


//##ModelId=3C0F6C1A001F
void MIOFile::Close( ) {
	if(!closed) {
		fclose(fp);
		closed = true;
		eof = true;
	}
}


//##ModelId=3C15F42303C4
std::string MIOFile::GetLastLineRead() {
	return lastLineRead;
}


//##ModelId=3C205AF20278
long MIOFile::Setpos(long offset) {
	long f;
	//clearerr(fp);
	f =  fseek(fp, offset, 0);
	if(feof(fp))
		eof=true;
	else
		eof=false;
	return f;
}


//##ModelId=3C205AF202C3
long MIOFile::Getpos() {
	return ftell(fp);
}


//##ModelId=3C205AF20313
bool MIOFile::MemBookmarkPos() {
	if((bookmarkPos = ftell(fp)) != -1)
		return true;
	else
		return false;
}


//##ModelId=3C205AF20334
bool MIOFile::SetLastBookmarkPos() {
	if(fseek(fp, bookmarkPos, SEEK_SET) == 0)
		return true;
	else
		return false;
}


//##ModelId=3A5A30B40368
bool MIOFile::IsEOF() {
	if(!closed)
		return eof;
	else
		return true;
}


//##ModelId=3C51324A022E
int MIOFile::SetFirstPos() {
	return Setpos(startPosition);
}


//##ModelId=3C51324A0299
bool MIOFile::Fchdir() {

	int len = filename.Length();
	int index;
	for(index=len-1; index>=0; index--) {
		//		char c = filename[index];
		if(filename[index] == '/')
			break;
	}
	char* path = new char[index+2];
	for(int i=0; i<=index; i++)
		path[i]=filename[i];
	path[index+1] = '\0';
	if(strlen(path) != 0)
	if(chdir(path)==0) {
		return true;
	}
	else
		return false;
	return true;
}


/** No descriptions */
//##ModelId=3C51324A0304
long MIOFile::Find(UChar_t b) {
	UChar_t f;
	while(!IsEOF()) {
		f = (UChar_t) GetByte();
		if(f == b)
			return Getpos();
	}
	return -1;
}


//##ModelId=3DA3E56D0244
bool MIOFile::WriteString(const std::string &str) {
	if(str != "")
		if(!closed)
			if(fprintf(fp,"%s",str.Data())<0)
				return false;
	return true;
}

bool MIOFile::WriteStringWithEndl(const std::string &str) {
	if(str != "")
		if(!closed) {
			str += "\n";
			if(fprintf(fp,"%s",str.Data())<0)
				return false;
			}
	return true;
}


bool MIOFile::WriteStringArray(const char* array[], const std::string &filename ) {
Bool_t ret;
UInt_t i = 0;

	if(filename != "")
		if(!Open(filename, "w"))
			return kFALSE;
	//the file is opened
	i = 0;
	while(array[i] != 0) {
		std::string s = array[i++];
		s += "\n";
		if(!WriteString(s))
			return kFALSE;
		}
	Close();
	return true;
}

void MIOFile::DeleteFile(const std::string &filename) {
	remove(filename.Data());
}
