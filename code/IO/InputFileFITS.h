#ifndef QL_IO_INPUTFILEFITS_H
#define QL_IO_INPUTFILEFITS_H

#include <cstdint>

#include "QLDefinition.h"
#include "InputFile.h"
#include "InputFileFilter.h"
#include <fitsio.h>

namespace ql_io {

//##ModelId=3FAF8EA503DF
class InputFileFITS : public InputFile {

	public:

		InputFileFITS();

		~InputFileFITS();

		//##Documentation
		//## \return true if file is opened, false if file is closed.
		virtual Bool_t Open(const std::string &filename);

		virtual Int_t GetNCols();

		virtual Long_t GetNRows();

		//##Documentation
		//## \return true if file is closed, false if file is opened.
		virtual Bool_t Close();

		virtual Bool_t MoveHeader(int header_number);

		//##Documentation
		//## Apply a filter to the current header
		virtual Bool_t ApplyFilter(const std::string &selectEvent);

		//##Documentation
		//## Remove the filter to the current header
		virtual void RemoveFilter();

		virtual Long_t GetNextRowPeriod(uint32_t timeColumnNumber, Long_t pos_first, DOUBLE_T end_time);

		virtual DOUBLE_T* GetTime(uint32_t timeColumnNumber, uint64_t start, uint64_t dim);

		virtual DOUBLE_T GetTime(uint32_t timeColumnNumber, uint64_t pos);

		virtual uint32_t GetIndexFirstColumn() { return 1;};

		virtual uint32_t GetIndexFirstRow() { return 1;};

		virtual uint32_t GetIndexFirstTableHeader() { return 1; };

		virtual void SetFilter(InputFileFilter* filter);

	public:

		//##Documentation
		//## 1B, 1 byte, unsigned. <br>Esempio di lettura di una colonna e N righe con tipo di formato fisso:<br>
		//##	uint8_t* gridlcdi = inputFile->Read_TBYTE(GRIDLCDI, first_pos_new, last_pos_new);<br>
		//## 	if(gridlcdi == kPointerNull) return inputFile->GetStatus();<br>
		//## Esempio di lettura di una colonna in formato variabile: <br>
		//## 	uint8_t* aclatcon = tempFile.Read_TBYTE(ACLATCON, j+1, j+1, 12);<br>
		//## 	if(aclatcon == kPointerNull) return tempFile.GetStatus();<br>
		//## in un ciclo per la lettura di tutte le righe sull'indice j.
		//## \param ncol number of column to be read
		//## \param frow number of the first row to be read (in the case of fixed format) or number of the row to be read (in the case of variable format)
		//## \param lrow number of the last row to be read (in the case of fixed format) or not used in the case of variable format
		//## \param nelements not used in the case of fixed format (it become equal to lrow - frow + 1) or number of the elements reads into the cell identified by the (ncol, frow) in the case of variable format
		virtual uint8_t* Read_TBYTE(int ncol, long frow, long lrow, Long_t nelements = 0);

		//##Documentation
		//## 1I, signed short
		virtual Short_t* Read_TSHORT(int ncol, long frow, long lrow, Long_t nelements = 0);

		virtual Int_t* Read_TINT(int ncol, long frow, long lrow, Long_t nelements = 0);

		//##Documentation
		//## 1J, signed int 32 bit
		virtual Long_t* Read_TINT32BIT(int ncol, long frow, long lrow, Long_t nelements = 0);

		virtual uint16_t* Read_TUSHORT(int ncol, long frow, long lrow, Long_t nelements = 0);

		virtual uint32_t* Read_TUINT(int ncol, long frow, long lrow, Long_t nelements = 0);

		virtual uint64_t* Read_TULONG(int ncol, long frow, long lrow, Long_t nelements = 0);

		virtual Float_t* Read_TFLOAT(int ncol, long frow, long lrow, Long_t nelements = 0);

		virtual Double_t* Read_TDOUBLE(int ncol, long frow, long lrow, Long_t nelements = 0);

		fitsfile *infptr;

	protected:

		virtual Bool_t GetFilteredRows(Long_t frow, Long_t nrows);

		InputFileFilter* filter;

		long n_good_rows;

		char* row_status;

		Long_t row_status_size;

};

}

#endif
