/***************************************************************************
    begin                : Sep 2 2013
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

#include<IO/InputFileText.h>
#include<sstream>
#include<fstream>
#include<iomanip>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>
#include<unistd.h>

BOOST_AUTO_TEST_CASE(input_file_text)
{
	qlbase::InputFileText file(",");

	// closing a not-opened file should raise an exception
	BOOST_CHECK_THROW(file.close(), qlbase::IOException);

	// opening an invalid file should raise an exception
	BOOST_CHECK_THROW(file.open("thisisnotafile"), qlbase::IOException);

	// opening a text file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.open("sample.txt"));

	// jumping on a header should work doing nothing
	BOOST_CHECK_NO_THROW(file.moveToHeader(10));

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

	// the file should be open
	BOOST_CHECK_EQUAL(file.isOpened(), true);

	// closing the file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.close());

	// the file should be closed
	BOOST_CHECK_EQUAL(file.isOpened(), false);

	// reading the first 4 rows from column 0 on a closed file should raise an exception
	BOOST_CHECK_THROW(rowsT1 = file.read32i(0, 0, 3), qlbase::IOException);
}
