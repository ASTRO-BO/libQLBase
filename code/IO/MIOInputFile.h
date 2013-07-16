#ifndef QLINPUTFILE_H_HEADER_INCLUDED_C04E1A23
#define QLINPUTFILE_H_HEADER_INCLUDED_C04E1A23

#include "QLDefinition.h"
#include <TObject.h>
#include <TArray.h>
#include <TString.h>
class MIOInputFileFilter;

//##ModelId=3FAF8D8C034F
//##Documentation
//## This class represent a generic input file with data organized as tabular data.
//## The responsibilities of this class are:
//## - read each column from a row to another row
//## L'indice della prima colonna, l'indice della prima riga e l'indice del primo header partono tutti
//## da 0. Utilizzare i rispettivi metodi per gli offset.
//## $Date: 2007/05/23 17:24:46 $
//## $Header: /home/repository/cvs/gtb/libQL2/code/MIOInputFile.h,v 1.2 2007/05/23 17:24:46 bulgarelli Exp $
//## $Id: MIOInputFile.h,v 1.2 2007/05/23 17:24:46 bulgarelli Exp $
//## $Revision: 1.2 $
//## \brief Insert here a brief description of the class

class MIOInputFile : public TObject {

	public:

		MIOInputFile( );

		virtual ~MIOInputFile();

		//il base header è l'header su cui devono essere contati il numero di eventi di un file
		virtual void SetBaseHeader(Int_t headerBase);

		//##ModelId=3FAF8E5F0371
		virtual Bool_t Open(TString filename) = 0;

		//##ModelId=3FAF8E850235
		virtual Bool_t Close() = 0;

		//##Documentation
		//## Si sposta nell'header specificato. La numerazione degli header parte da 0.
		//## Specificare l'offset con il metdo GetIndexFirstTableHeader().
		virtual Bool_t MoveHeader(int header_number) = 0;
		
		//##Documentation
		//## Apply a filter to the current header
		virtual Bool_t ApplyFilter(TString selectEvent) { return kFALSE; };
		
		//##Documentation
		//## Remove the filter to the current header
		virtual void RemoveFilter() {};
		

		virtual Int_t GetNCols() { return ncols; };

		virtual Long_t GetNRows() { return nrows; };

		virtual Long_t GetNEvents();

		virtual Long_t GetNextRowPeriod(UInt_t timeColumnNumber, Long_t pos_first, DOUBLE_T end_time) = 0;

		virtual DOUBLE_T* GetTime(UInt_t timeColumnNumber, ULong_t start, ULong_t dim) = 0;

		virtual DOUBLE_T GetTime(UInt_t timeColumnNumber, ULong_t pos) = 0;

		virtual Bool_t IsOpened();

		virtual Int_t GetStatus() { return status; };

		virtual TString GetFileName() { return filename; };
		
		virtual void SetFileName(TString filename) { this->filename = filename; };

		//##Documentation
		//## Indica da dove deve partire l'indice della prima colonna nel file (0 o 1)
		virtual UInt_t GetIndexFirstColumn() = 0;

		//##Documentation
		//## Indica da dove deve partire l'indice della prima riga nel file (0 o 1)		
		virtual UInt_t GetIndexFirstRow() = 0;
		
		//##Documentation
		//## Indica da dove deve partire l'indice del primo header nei file strutturati in tabella (0 o 1)		
		virtual UInt_t GetIndexFirstTableHeader() = 0;
		
		virtual void SetFilter(MIOInputFileFilter* filter) {};

	public:

		virtual Long_t GetNRowReads() { return nRowsRead; };

		virtual UChar_t* Read_TBYTE(int ncol, long frow, long lrow, Long_t nelements = 0) = 0;

		virtual Short_t* Read_TSHORT(int ncol, long frow, long lrow, Long_t nelements = 0) = 0;

		virtual Int_t* Read_TINT(int ncol, long frow, long lrow, Long_t nelements = 0) = 0;

		virtual Long_t* Read_TINT32BIT(int ncol, long frow, long lrow, Long_t nelements = 0) = 0;

		virtual UShort_t* Read_TUSHORT(int ncol, long frow, long lrow, Long_t nelements = 0) = 0;

		virtual UInt_t* Read_TUINT(int ncol, long frow, long lrow, Long_t nelements = 0) = 0;

		virtual ULong_t* Read_TULONG(int ncol, long frow, long lrow, Long_t nelements = 0) = 0;

		virtual Float_t* Read_TFLOAT(int ncol, long frow, long lrow, Long_t nelements = 0) = 0;

		virtual Double_t* Read_TDOUBLE(int ncol, long frow, long lrow, Long_t nelements = 0) = 0;

	protected:

		//##ModelId=3FAF8EE401B9
		TString filename;

		Bool_t opened;

		//##Documentation
		//## Number of columns of the current file
		Int_t ncols;

		//##Documentation
		//## Number of rows of the current file
		Long_t nrows;

		//##Documentation
		//## Number of rows of the last reading;
		Long_t nRowsRead;

		//##Documentation
		//## The status of the reading
		Int_t status;
		
		Bool_t applyFilter;
		
		TString selectEvent;

		//the header che contiene gli eventi da elaborare. Usato da GetNEvents()
		Int_t headerBase;

};
#endif							 /* QLINPUTFILE_H_HEADER_INCLUDED_C04E1A23 */
