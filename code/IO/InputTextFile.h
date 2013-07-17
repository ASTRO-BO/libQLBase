#ifndef QL_IO_INPUTTEXTFILE_H
#define QL_IO_INPUTTEXTFILE_H

#include "InputFile.h"
#include <fstream>
#include <cstdint>

namespace ql_io {

//##ModelId=3FAF8D8C034F
//##Documentation
//## This class represent a generic input file with data organized as tabular data.
//## The responsibilities of this class are:
//## - read each column from a row to another row
//## $Date: 2007/05/23 17:24:46 $
//## $Header: /home/repository/cvs/gtb/libQL2/code/QLInputTextFile.h,v 1.9 2007/05/23 17:24:46 bulgarelli Exp $
//## $Id: InputTextFile.h,v 1.9 2007/05/23 17:24:46 bulgarelli Exp $
//## $Revision: 1.9 $
//## \brief Insert here a brief description of the class

class InputTextFile : public InputFile {

	public:

		InputTextFile(const std::string &separator = std::string(" "));

		virtual ~InputTextFile();

		//##ModelId=3FAF8E5F0371
		Bool_t Open(const std::string &fileName);

		//##ModelId=3FAF8E850235
		Bool_t Close();

		Bool_t MoveHeader(int header_number) { return kTRUE; };

		int64_t GetNextRowPeriod(uint32_t timeColumnNumber, int64_t pos_first, DOUBLE_T end_time);

		DOUBLE_T GetTime(uint32_t timeColumnNumber, uint64_t pos);

		Bool_t IsOpened();

		DOUBLE_T* GetTime(uint32_t timeColumnNumber, uint64_t start, uint64_t dim);

		uint8_t* Read_TBYTE(int ncol, long frow, long lrow, int64_t nelements = 0);

		int16_t* Read_TSHORT(int ncol, long frow, long lrow, int64_t nelements = 0);

		int32_t* Read_TINT(int ncol, long frow, long lrow, int64_t nelements = 0);

		int64_t* Read_TINT32BIT(int ncol, long frow, long lrow, int64_t nelements = 0);

		uint16_t* Read_TUSHORT(int ncol, long frow, long lrow, int64_t nelements = 0);

		uint32_t* Read_TUINT(int ncol, long frow, long lrow, int64_t nelements = 0);

		uint64_t* Read_TULONG(int ncol, long frow, long lrow, int64_t nelements = 0);

		float* Read_TFLOAT(int ncol, long frow, long lrow, int64_t nelements = 0);

		double* Read_TDOUBLE(int ncol, long frow, long lrow, int64_t nelements = 0);

		virtual uint32_t GetIndexFirstColumn() { return 0;};

		virtual uint32_t GetIndexFirstRow() { return 0;};

		virtual uint32_t GetIndexFirstTableHeader() { return 0; };

	protected:					 // Protected attribute
		std::ifstream*  fileStream;
		std::string separator;

	protected:					 // Protected methods
		void pointTo(int line);
		bool findField(std::string& line, int& first, int& last, int pos = 0);
		bool reopen();
		bool test(int ncol, long frow, long& lrow);

	private:					 // Private methods
		void _printState();
};

}

#endif
