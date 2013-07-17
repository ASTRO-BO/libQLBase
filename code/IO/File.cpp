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

#include <unistd.h>
#include "File.h"

namespace ql_io {

#define EOI -1

unsigned long File::byte_read = 0;
unsigned long File::char_read = 0;

File::File(long startP) {
//	bigendian = false; //si presume che la macchina sia little endian
	startPosition = startP;
	filename = "";
	closed = true;
}

bool File::IsClosed() {
	return closed;
}

bool File::Open(const std::string &filename, char* mode) {
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

int File::GetByte() {
	int c;
	if(!closed && !eof) {
		c = fgetc(fp);
		if(c == EOF) {
			eof = true;
			//close();
			c = EOI;
		}
		File::byte_read++;
		return c;
	}
	else {
		return EOI;
	}
}

std::string File::GetLine() {
	//char* s = new char[500];
	static char s[512];
	s[0]=0;
	//	static unsigned long dimalloc = 0;
	char c;
	int i = 0;

	if(!closed) {
		c=fgetc(fp);
		File::char_read++;
		while( c != '\n' && c != EOF ) {
			s[i] = c;
			//printf("%c", c);
			i++;
			if(i>=512)
				return s;
			c=fgetc(fp);
			File::char_read++;
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

std::string File::GetConfigurationLine() {
	std::string temp;

	temp = GetLine();
	while(strstr(temp, "--") !=  NULL) {
		temp = GetLine();
	}
	return temp;
}

std::string File::GetLine(const std::string &s) {
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

void File::Close( ) {
	if(!closed) {
		fclose(fp);
		closed = true;
		eof = true;
	}
}

std::string File::GetLastLineRead() {
	return lastLineRead;
}

long File::Setpos(long offset) {
	long f;
	//clearerr(fp);
	f =  fseek(fp, offset, 0);
	if(feof(fp))
		eof=true;
	else
		eof=false;
	return f;
}

long File::Getpos() {
	return ftell(fp);
}

bool File::MemBookmarkPos() {
	if((bookmarkPos = ftell(fp)) != -1)
		return true;
	else
		return false;
}

bool File::SetLastBookmarkPos() {
	if(fseek(fp, bookmarkPos, SEEK_SET) == 0)
		return true;
	else
		return false;
}

bool File::IsEOF() {
	if(!closed)
		return eof;
	else
		return true;
}

int File::SetFirstPos() {
	return Setpos(startPosition);
}

bool File::Fchdir() {

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

long File::Find(uint8_t b) {
	uint8_t f;
	while(!IsEOF()) {
		f = (uint8_t) GetByte();
		if(f == b)
			return Getpos();
	}
	return -1;
}

bool File::WriteString(const std::string &str) {
	if(str != "")
		if(!closed)
			if(fprintf(fp,"%s",str.Data())<0)
				return false;
	return true;
}

bool File::WriteStringWithEndl(const std::string &str) {
	if(str != "")
		if(!closed) {
			str += "\n";
			if(fprintf(fp,"%s",str.Data())<0)
				return false;
			}
	return true;
}

bool File::WriteStringArray(const char* array[], const std::string &filename ) {
bool ret;
uint32_t i = 0;

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

void File::DeleteFile(const std::string &filename) {
	remove(filename.Data());
}

}
