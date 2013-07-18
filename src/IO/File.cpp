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
#include <stdio.h>
#include "File.h"

namespace qlbase {

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

bool File::Open(const std::string &filename, const char* mode) {
	fp = fopen(filename.c_str(), mode);

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

    lastLineRead = std::string(s);
	return lastLineRead;
}

std::string File::GetConfigurationLine() {
	std::string temp;

	temp = GetLine();
	while(temp.find("--") != std::string::npos) {
		temp = GetLine();
	}
	return temp;
}

std::string File::GetLine(const std::string &s) {
	std::string line;
	line = GetLine();

	while(line.find(s) != std::string::npos) {
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

	int pos = filename.find_last_of('/');
	if(pos != std::string::npos)
	{
		std::string path = filename.substr(pos);
		if(chdir(path.c_str()) != 0)
			return false;
	}

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
	if(str == "" || close)
		return false;

	if(fprintf(fp,"%s",str.c_str())<0)
		return false;

	return true;
}

bool File::WriteStringWithEndl(const std::string &str) {
	if(str == "" || close)
		return false;

	std::string temp = str;
	temp.append("\n");
	if(fprintf(fp,"%s",temp.c_str())<0)
		return false;

	return true;
}

bool File::WriteStringArray(const char* array[], const std::string &filename ) {
	bool ret;
	uint32_t i = 0;

	if(filename == "")
		return false;

	if(!Open(filename, "w"))
		return false;

	i = 0;
	while(array[i] != 0) {
		std::string s = array[i++];
		s.append("\n");
		if(!WriteString(s))
			return false;
	}

	Close();

	return true;
}

void File::DeleteFile(const std::string &filename) {
	remove(filename.c_str());
}

}
