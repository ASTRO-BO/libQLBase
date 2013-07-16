//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MIOOUTPUTFILEMIOOutputFileFITS_H
#define MIOOUTPUTFILEMIOOutputFileFITS_H

#include  <fitsio.h>
#include "QLDefinition.h"

class MIOOutputFileFITS
{
    public:

        MIOOutputFileFITS();

        virtual ~MIOOutputFileFITS();
	
	virtual bool open();
	
	virtual bool close();
	
	virtual void setMasterHeader(int masterheadnum);

        virtual void printerror(int status);
	
	virtual void writeChksum(int headnum);
	
	virtual void writeInitHeaderKey(int header);
	
	virtual void writeEndHeaderKey(int header);

        virtual char* setFileName(char* filenamebase = 0);
	
	virtual int getColNum(char* nomecol);
	
	virtual long getRowNum();
	
	virtual void SetCurrentRowToWrite(dword cr) { currentrow = cr; };
	
	virtual bool flush_file();
	
	virtual void writeHistoryFile();
		
        template < typename TIPO >
        int writeCell(  int rownum , TIPO valore  , char * nomecol); 

	template < typename TIPO >
        int writeCell(  int rownum , TIPO valore  , int colnum); 
	
	long findFirstRow(char* expr);
	
	bool changeHeader(int headnum);

protected:
	     
        int openOutFits(char * outfitsname , char * templatefile); 

        int printerror(int codice,char* messaggio,int status_fitsio);
	
        int flushCloseandExit(); // flush fits file and exit 	
	
	// Pointer to fits file
        fitsfile** fptr;
	
	// File name
        char* filename;
	
        // Indicates the current row of data to start for writing the events data
        dword currentrow; //ex firstrow

        // Indicates the last row flushed	
	dword lastrow_flushed;	
	
	bool verbose;
	
	//flush ogni n righe
	int flush_rows; 
	
	bool opened;

	int status;
	
	long rownum;
	
	int colnum;
	
	long findfirstrow;
	
	int masterheadnum;
};


template < typename TIPO >  
int  MIOOutputFileFITS::writeCell( int rownum , TIPO valore , char * nomecol) 
 {
 	return writeCell(rownum, valore, getColNum(nomecol));
 } 
 
 template < typename TIPO >  
int  MIOOutputFileFITS::writeCell( int rownum , TIPO valore , int colnum) 
 {
	int status_fits = 0  ; 
	
	int firstelem=1 ; 
	int tipodati ;
	
	// determino il tipo del dato fornito in argomento , in base a questo fisso tipodati
	// tipodati serve al fitsio per fare le conversioni automatiche  
	
	// ATTENZIONE, ai codici di tipo di dato di cfitsio :
	//       TBYTE = 1B = 8 bit = 1 byte ,  char in gcc Linux i386
	//       TLONG == TINT32BIT = 1J = int in gcc 
	//       TINT = 1I = int 32 bits in gcc  Linux  i386 
	//  TLONGLONG = 1K = 64 bits = long long int in gcc Linux  i386
	
	
	if( typeid(valore).name() == typeid(unsigned char).name() )      tipodati = TBYTE ; //packetlib byte - 1 byte unsigned (1B)
	else if( typeid(valore).name() == typeid(short int).name() )      tipodati = TSHORT ; //2 byte signed (1I)
	else if( typeid(valore).name() == typeid(unsigned short).name() )      tipodati = TUSHORT ; //packetlib word - 2 byte unsigned short (1U)
	else if( typeid(valore).name() == typeid(long).name() )     tipodati = TLONG ;  //32 bit long signed integer (1J)   
	else if( typeid(valore).name() == typeid(unsigned long).name() )      tipodati = TULONG ; //packetlib dword - 4 byte unsigned long (1V)
	else if( typeid(valore).name() == typeid(int).name() )               tipodati = TINT32BIT  ; //4 byte (32 bit) signed (1J)
	else if( typeid(valore).name() == typeid(unsigned int).name() )      tipodati = TUINT ;  // 4 byte unsigned int (1V)
	else if( typeid(valore).name() == typeid(float).name() )            tipodati = TFLOAT ; //4 byte real (1E)
	else if( typeid(valore).name() == typeid(double).name() )            tipodati = TDOUBLE ;   //8 byte real (1D)  
	else if( typeid(valore).name() == typeid(long long int).name() )     tipodati = TLONGLONG ;  //64 bit long signed integer (1K)  
	else
	{
		cerr << " ERROR: data type unknown into writeCell: ";
		cerr << " writing command ignored. "  << endl ;
		
		return 1 ;
	} 	
		
	if(status_fits)
	{ 
		cerr << endl <<" MAYBE WRONG COL : " << colnum << " value: " << valore << endl ; 
		printerror( colnum," Looking for col number ",status_fits );
	}
	fits_write_col(*fptr,tipodati,colnum,rownum,firstelem, (long)1, &valore, &status_fits);
	if(status_fits)   
	{
		cerr << endl<< " MAYBE WRONG VALUE IN : " << colnum << " value: " << valore << endl ; 
		printerror( colnum," Writing fits col: ",status_fits );  
	} 
 };


//========================================================================

class MIOOutputFileFITSBinaryTable : public MIOOutputFileFITS
{
	public:
        	
        	MIOOutputFileFITSBinaryTable(int ncol_header1, int ncol_header2 = 0, int ncol_header3 = 0, int ncol_header4 = 0, int ncol_header5 = 0, int ncol_header6 = 0);

        	virtual ~MIOOutputFileFITSBinaryTable();
		
        	virtual bool init();

		virtual bool close();

		virtual char* getValue(char* str);
		
	protected:

        	int ncol_header1;
        	int ncol_header2;
		int ncol_header3;
        	int ncol_header4;
		int ncol_header5;
		int ncol_header6;
		
		char *extname1;
		char **ttype1;
		char **tform1;
		char **tunit1;
		int* pfind1;    
    		char ** tycom1;  
        
		char *extname2;
		char **ttype2;
		char **tform2;
		char **tunit2;
		int* pfind2;
		char ** tycom2;
				
		char *extname3;
		char **ttype3;
		char **tform3;
		char **tunit3;		
		int* pfind3;
		char ** tycom3;

		char *extname4;
		char **ttype4;
		char **tform4;
		char **tunit4;		
		int* pfind4;
		char ** tycom4;

		char *extname5;
		char **ttype5;
		char **tform5;
		char **tunit5;		
		int* pfind5;
		char ** tycom5;
		
		char *extname6;
		char **ttype6;
		char **tform6;
		char **tunit6;		
		int* pfind6;
		char ** tycom6;		
        
		long nrows_event_hdu;
    		long nrows_event_hdu2;  // row index for ext. 2
    		long nrows_event_hdu3;  // row index for ext. 3		
		long nrows_event_hdu4;  // row index for ext. 4		
		long nrows_event_hdu5;  // row index for ext. 5		
		long nrows_event_hdu6;  // row index for ext. 6		
        
		int tm_apid;
        	int tm_type;
        	int tm_subtype;
		
		//OBT letto (quello nell'header AGILE)
		double dfhobt_sec;
		double dfhobt_us;
		double dfhobt_time;
		
		//sec e ms letti con l'ultima chiamata a writeRawOBTFormat
		unsigned long last_sec_read;
		unsigned long last_ms_read;



};




#endif
