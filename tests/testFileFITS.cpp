/***************************************************************************
    begin                : Jul 29 2013
    copyright            : (C) 2013 Andrea Zoli
    email                : zoli@iasfbo.inaf.it
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software for non commercial purpose              *
 *   and for public research institutes; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License.          *
 *   For commercial purpose see appropriate license terms                  *
 *                                                                         *
 ***************************************************************************/

#include<IO/InputFileFITS.h>
#include<IO/OutputFileFITS.h>
#include<sstream>
#include<fstream>
#include<iomanip>
#include<iostream>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>
#include<unistd.h>

static const std::string keywords[] = {
"XTENSION= 'BINTABLE'           / binary table extension",
"BITPIX  =                    8 / 8-bit bytes",
"NAXIS   =                    2 / 2-dimensional binary table",
"NAXIS1  =                   93 / width of table in bytes",
"NAXIS2  =                   10 / number of rows in table",
"PCOUNT  =                    0 / size of special data area",
"GCOUNT  =                    1 / one data group (required keyword)",
"TFIELDS =                   12 / number of fields in each row",
"TTYPE1  = 'field0  '           / label for field   1",
"TFORM1  = '1J      '           / data format of field: 4-byte INTEGER",
"TUNIT1  = 'mph     '           / physical unit of field",
"TTYPE2  = 'field1  '           / label for field   2",
"TFORM2  = '1B      '           / data format of field: BYTE",
"TUNIT2  = 'cm      '           / physical unit of field",
"TTYPE3  = 'field2  '           / label for field   3",
"TFORM3  = '1J      '           / data format of field: 4-byte INTEGER",
"TUNIT3  = 'mph     '           / physical unit of field",
"TTYPE4  = 'field3  '           / label for field   4",
"TFORM4  = '1B      '           / data format of field: BYTE",
"TUNIT4  = 'cm      '           / physical unit of field",
"TTYPE5  = 'field4  '           / label for field   5",
"TFORM5  = '1J      '           / data format of field: 4-byte INTEGER",
"TUNIT5  = 'mph     '           / physical unit of field",
"TTYPE6  = 'field5  '           / label for field   6",
"TFORM6  = '1B      '           / data format of field: BYTE",
"TUNIT6  = 'cm      '           / physical unit of field",
"TTYPE7  = 'field6  '           / label for field   7",
"TFORM7  = '1J      '           / data format of field: 4-byte INTEGER",
"TUNIT7  = 'mph     '           / physical unit of field",
"TTYPE8  = 'field7  '           / label for field   8",
"TFORM8  = '1B      '           / data format of field: BYTE",
"TUNIT8  = 'cm      '           / physical unit of field",
"TTYPE9  = 'field8  '           / label for field   9",
"TFORM9  = '1J      '           / data format of field: 4-byte INTEGER",
"TUNIT9  = 'mph     '           / physical unit of field",
"TTYPE10 = 'field9  '           / label for field  10",
"TFORM10 = '1B      '           / data format of field: BYTE",
"TUNIT10 = 'cm      '           / physical unit of field",
"TTYPE11 = 'fvector '           / label for field  11",
"TFORM11 = '12E     '           / data format of field: 4-byte REAL",
"TUNIT11 = 'cms     '           / physical unit of field",
"TTYPE12 = 'fstring '           / label for field  12",
"TFORM12 = '20A     '           / data format of field: ASCII Character",
"TUNIT12 = 'desc    '           / physical unit of field",
"EXTNAME = 'testing binary table' / name of this binary table extension",
"TDIM12  = '(20,1)  '           / size of the multidimensional array" };

BOOST_AUTO_TEST_CASE(input_file_fits)
{
	qlbase::InputFileFITS file;

	// closing a not-opened file should raise an exception
	BOOST_CHECK_THROW(file.close(), qlbase::IOException);

	// opening an invalid file should raise an exception
	BOOST_CHECK_THROW(file.open("thisisnotafile"), qlbase::IOException);

	// opening a FITS file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.open("sample.fits"));

	// the number of headers should be 3
	int num;
	BOOST_CHECK_NO_THROW(num = file.getHeadersNum());
	BOOST_CHECK_EQUAL(num, 3);

	// jumping on a bad chunck should raise an exception
	BOOST_CHECK_THROW(file.moveToHeader(10), qlbase::IOException);

	// jumping on an existing chunck shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.moveToHeader(1));

	// reading the first 4 rows from column 0 shouldn't raise an exception
	std::vector<int32_t> rowsT1;
	BOOST_CHECK_NO_THROW(rowsT1 = file.read32i(0, 0, 3));

	// and their values should be n. 1, 2, 3, 4
	std::vector<int64_t> expectedT1(4);
	expectedT1[0] = 0;
	expectedT1[1] = 1;
	expectedT1[2] = 2;
	expectedT1[3] = 3;
	BOOST_CHECK_EQUAL_COLLECTIONS(rowsT1.begin(), rowsT1.end(), expectedT1.begin(), expectedT1.end());

	// reading first 10 rows from the 8th column shouldn't raise an exception
	std::vector<uint8_t> rowsT2;
	BOOST_CHECK_NO_THROW(rowsT2 = file.readu8i(7, 0, 9));

	// the read packets should be the following
	std::vector<uint8_t> expectedT2(10);
	expectedT2.resize(10);
	expectedT2[0] = 70;
	expectedT2[1] = 71;
	expectedT2[2] = 72;
	expectedT2[3] = 73;
	expectedT2[4] = 74;
	expectedT2[5] = 75;
	expectedT2[6] = 76;
	expectedT2[7] = 77;
	expectedT2[8] = 78;
	expectedT2[9] = 79;
	BOOST_CHECK_EQUAL_COLLECTIONS(rowsT2.begin(), rowsT2.end(), expectedT2.begin(), expectedT2.end());

	// reading the entire 11 column 8th column shouldn't raise an exception
	std::vector< std::vector<float> > rowsT3;
	BOOST_CHECK_NO_THROW(rowsT3 = file.read32fv(10, 0, 9, 12));

	// the read packets should be the following
	for(unsigned int row=0; row<10; row++)
	{
		std::vector<float> expectedT3(12, (float)row);
		for(unsigned int i=0; i<12; i++)
			BOOST_REQUIRE_CLOSE( rowsT3[row][i], expectedT3[i], 0.001 );
	}

	// reading the 12 column shouldn't raise an exception
	std::vector< std::vector<char> > rowsT4;
	BOOST_CHECK_NO_THROW(rowsT4 = file.readString(11, 0, 9, 20));

	// the rows strings should be 'aaaaaaaaaaaaaaaaaaaa', 'bbbbbbbbbbbbbbbbbbbb', ...
	std::vector< std::vector<char> > expectedT4(10);
	for(unsigned int row=0; row<10; row++)
	{
		std::vector<char> expectedT4(20, 'a'+row);
		for(unsigned int i=0; i<20; i++)
			BOOST_CHECK_EQUAL(rowsT4[row][i], expectedT4[i]);
	}

	// the file should be open
	BOOST_CHECK_EQUAL(file.isOpened(), true);

	// jumping on an existing chunck shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.moveToHeader(2));

	// reading the image shouldn't raise an exception
	qlbase::Image<float> img;
	BOOST_CHECK_NO_THROW(img = file.readImage32f());
	std::stringstream ss;

	// image values should be like the ones stored in the img.csv file.
	std::vector<std::string> rows;
	for(int i=0; i<img.data.size(); i++)
	{

		ss << std::fixed << std::setprecision(5) << img.data[i];

		if(((i+1) % img.sizes[1]) == 0 && i != 0)
		{
			rows.push_back(ss.str());
			ss.str("");
		}
		else
			ss << " ";
	}
	std::ifstream imgfile("img.csv");
	std::vector<std::string> rowsExpected;
	std::string row;
	while(std::getline(imgfile, row))
		rowsExpected.push_back(row);
	BOOST_CHECK_EQUAL_COLLECTIONS(&rows[0], &rows[rows.size()], &rowsExpected[0], &rowsExpected[rowsExpected.size()]);

	// closing the file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.close());

	// the file should be closed
	BOOST_CHECK_EQUAL(file.isOpened(), false);

	// reading the first 4 rows from column 0 on a closed file should raise an exception
	BOOST_CHECK_THROW(rowsT1 = file.read32i(0, 0, 3), qlbase::IOException);

	// the fits header should be the same of the keywords string vector.
	BOOST_CHECK_NO_THROW(file.open("sample.fits"));
	for(int i=0; i<file.getKeywordNum(); i++)
		BOOST_CHECK(file.getKeyword(i).compare(keywords[i]) == 0);
}

BOOST_AUTO_TEST_CASE(output_file_fits)
{
	qlbase::OutputFileFITS ofile;

	// closing a not-opened file should raise an exception
	BOOST_CHECK_THROW(ofile.close(), qlbase::IOException);

	// creating a new fits file should not raise an exception
	BOOST_CHECK_NO_THROW(ofile.create("testing.fits"));

	// going to the first chunk should not raise an error
	BOOST_CHECK_NO_THROW(ofile.moveToHeader(0));

	// writing a new binary table on first header should raise an error
	std::vector<qlbase::field> fields(10);
	std::stringstream ss;
	for(unsigned int i=0; i<fields.size(); i++)
	{
		ss.str("");
		ss << "field" << i;
		fields[i].name = ss.str();
		(i % 2 == 0) ? fields[i].type=qlbase::INT32 : fields[i].type=qlbase::UNSIGNED_INT8;
		fields[i].vsize = 1;
		(i % 2 == 0) ? fields[i].unit="mph" : fields[i].unit="cm";
	}
    // 11 column
	qlbase::field vectorfield;
	vectorfield.name = "fvector";
	vectorfield.type = qlbase::FLOAT;
	vectorfield.vsize = 12;
	vectorfield.unit = "cms";
	fields.push_back(vectorfield);

    // 12 column
	vectorfield.name = "fstring";
	vectorfield.type = qlbase::STRING;
	vectorfield.vsize = 20;
	vectorfield.unit = "desc";
	fields.push_back(vectorfield);

	BOOST_CHECK_NO_THROW(ofile.createTable("testing binary table", fields));

	// going to the second chunk should not raise an error
	BOOST_CHECK_NO_THROW(ofile.moveToHeader(1));

	// closing the file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(ofile.close());

	// the file should be closed
	BOOST_CHECK_EQUAL(ofile.isOpened(), false);

	// open the fits file should not raise an exception
	BOOST_CHECK_NO_THROW(ofile.open("testing.fits"));

	// going to the second chunk should not raise an exception
	BOOST_CHECK_NO_THROW(ofile.moveToHeader(1));

	// writing columns not raise an exception
	const int NROW = 10;
	std::vector< std::vector<int32_t> > colsi(5);
	std::vector< std::vector<uint8_t> > colsb(5);

	for(unsigned int i=0; i<5; i++)
	{
		for(unsigned int row=0; row<NROW; row++)
		{
			colsi[i].push_back(i*2*NROW+row);
			colsb[i].push_back((i*2+1)*NROW+row);
		}

		BOOST_CHECK_NO_THROW(ofile.write32i(i*2, colsi[i], 0, NROW-1));
		BOOST_CHECK_NO_THROW(ofile.writeu8i(i*2+1, colsb[i], 0, NROW-1));
	}
	std::vector< std::vector<float> > vectors;
	for(unsigned int row=0; row<NROW; row++)
	{
		std::vector<float> v(12, (float)row);
		vectors.push_back(v);
	}
	BOOST_CHECK_NO_THROW(ofile.write32fv(10, vectors, 0, NROW-1));

	std::vector< std::vector<char> > vectorStr;
	for(unsigned int row=0; row<NROW; row++)
	{
		std::vector<char> v(20, 'a'+row);
		vectorStr.push_back(v);
	}
	BOOST_CHECK_NO_THROW(ofile.close());
	BOOST_CHECK_NO_THROW(ofile.open("testing.fits"));
	BOOST_CHECK_NO_THROW(ofile.writeString(11, vectorStr, 0, NROW-1));

	BOOST_CHECK_NO_THROW(ofile.writeKeyword("field1", "100", "the first field"));
	BOOST_CHECK_NO_THROW(ofile.writeKeyword("field2", "2", "the second field"));
	BOOST_CHECK_NO_THROW(ofile.writeKeyword("field3", "3", "the third field"));
	BOOST_CHECK_NO_THROW(ofile.writeKeyword("field4", "4", "the fourth field"));
	BOOST_CHECK_NO_THROW(ofile.writeKeyword("field5", "5", "the fifth field"));
	BOOST_CHECK_NO_THROW(ofile.writeKeyword("field1", "1", "the sixth field"));

	// closing the file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(ofile.close());

	unlink("testing.fits");
}
