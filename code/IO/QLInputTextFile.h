#ifndef QLINPUTTEXTFILE_H_HEADER_INCLUDED_C04E1A23
#define QLINPUTTEXTFILE_H_HEADER_INCLUDED_C04E1A23

#include "MIOInputFile.h"
#include <fstream>

//##ModelId=3FAF8D8C034F
//##Documentation
//## This class represent a generic input file with data organized as tabular data.
//## The responsibilities of this class are:
//## - read each column from a row to another row
//## $Date: 2007/05/23 17:24:46 $
//## $Header: /home/repository/cvs/gtb/libQL2/code/QLInputTextFile.h,v 1.9 2007/05/23 17:24:46 bulgarelli Exp $
//## $Id: QLInputTextFile.h,v 1.9 2007/05/23 17:24:46 bulgarelli Exp $
//## $Revision: 1.9 $
//## \brief Insert here a brief description of the class

class QLInputTextFile : public MIOInputFile {

	public:

		QLInputTextFile(const std::string &separator = std::string(" "));

		virtual ~QLInputTextFile();

		//##ModelId=3FAF8E5F0371
		Bool_t Open(const std::string &fileName);

		//##ModelId=3FAF8E850235
		Bool_t Close();

		Bool_t MoveHeader(int header_number) { return kTRUE; };

		Long_t GetNextRowPeriod(UInt_t timeColumnNumber, Long_t pos_first, DOUBLE_T end_time);

		DOUBLE_T GetTime(UInt_t timeColumnNumber, ULong_t pos);

		Bool_t IsOpened();

		DOUBLE_T* GetTime(UInt_t timeColumnNumber, ULong_t start, ULong_t dim);

		UChar_t* Read_TBYTE(int ncol, long frow, long lrow, Long_t nelements = 0);

		Short_t* Read_TSHORT(int ncol, long frow, long lrow, Long_t nelements = 0);

		Int_t* Read_TINT(int ncol, long frow, long lrow, Long_t nelements = 0);

		Long_t* Read_TINT32BIT(int ncol, long frow, long lrow, Long_t nelements = 0);

		UShort_t* Read_TUSHORT(int ncol, long frow, long lrow, Long_t nelements = 0);

		UInt_t* Read_TUINT(int ncol, long frow, long lrow, Long_t nelements = 0);

		ULong_t* Read_TULONG(int ncol, long frow, long lrow, Long_t nelements = 0);

		Float_t* Read_TFLOAT(int ncol, long frow, long lrow, Long_t nelements = 0);

		Double_t* Read_TDOUBLE(int ncol, long frow, long lrow, Long_t nelements = 0);

		virtual UInt_t GetIndexFirstColumn() { return 0;};

		virtual UInt_t GetIndexFirstRow() { return 0;};
		
		virtual UInt_t GetIndexFirstTableHeader() { return 0; };

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
#endif							 /* QLINPUTTEXTFILE_H_HEADER_INCLUDED_C04E1A23 */
