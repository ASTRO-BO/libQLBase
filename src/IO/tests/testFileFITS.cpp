#include<IO/InputFileFITS.h>
#include<IO/OutputFileFITS.h>
#include<sstream>
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(input_file_fits)
{
	qlbase::InputFileFITS file;

	// closing a not-opened file should raise an exception
	BOOST_CHECK_THROW(file.close(), qlbase::IOException);

	// opening an invalid file should raise an exception
	BOOST_CHECK_THROW(file.open("thisisnotafile"), qlbase::IOException);

	// opening a FITS file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.open("PKP015933_1_3901_000.lv1"));

	// jumping on a bad chunck should raise an exception
	BOOST_CHECK_THROW(file.jumpToChunk(10), qlbase::IOException);

	// jumping on an existing chunck shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.jumpToChunk(1));

	// reading the first 4 rows from column 0 shouldn't raise an exception
	std::vector<int64_t> rowsT1;
	BOOST_CHECK_NO_THROW(rowsT1 = file.read64i(0, 0, 3));

	// and their values should be n. 1, 2, 3, 4
	std::vector<int64_t> expectedT1(4);
	expectedT1[0] = 1;
	expectedT1[1] = 2;
	expectedT1[2] = 3;
	expectedT1[3] = 4;
	BOOST_CHECK_EQUAL_COLLECTIONS(&rowsT1[0], &rowsT1[rowsT1.size()], &expectedT1[0], &expectedT1[expectedT1.size()]);

	// reading first 10 rows from the 12th column shouldn't raise an exception
	std::vector<uint8_t> rowsT2;
	BOOST_CHECK_NO_THROW(rowsT2 = file.readu8i(11, 0, 9));

	// the read packets should be the following
	std::vector<uint8_t> expectedT2(10);
	expectedT2.resize(10);
	expectedT2[0] = 154;
	expectedT2[1] = 180;
	expectedT2[2] = 181;
	expectedT2[3] = 185;
	expectedT2[4] = 186;
	expectedT2[5] = 190;
	expectedT2[6] = 191;
	expectedT2[7] = 195;
	expectedT2[8] = 199;
	expectedT2[9] = 203;
	BOOST_CHECK_EQUAL_COLLECTIONS(&rowsT2[0], &rowsT2[rowsT2.size()], &expectedT2[0], &expectedT2[expectedT2.size()]);

	// the file should be open
	BOOST_CHECK_EQUAL(file.isOpened(), true);

	// closing the file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.close());

	// the file should be closed
	BOOST_CHECK_EQUAL(file.isOpened(), false);

	// reading the first 4 rows from column 0 on a closed file should raise an exception
	BOOST_CHECK_THROW(rowsT1 = file.read64i(0, 0, 3), qlbase::IOException);
}

BOOST_AUTO_TEST_CASE(output_file_fits)
{
	qlbase::OutputFileFITS ofile;

	// closing a not-opened file should raise an exception
//	BOOST_CHECK_THROW(ofile.close(), qlbase::IOException); // FIXME THIS CAUSE THE SUITE TO CRASH

	// creating a new fits file should not raise an exception
	BOOST_CHECK_NO_THROW(ofile.create("testing.fits"));

	// going to the first chunk should not raise an error
	BOOST_CHECK_NO_THROW(ofile.jumpToChunk(0));

	// writing a new binary table on first header should raise an error
	std::vector<qlbase::field> fields(10);
	std::stringstream ss;
	for(unsigned int i=0; i<fields.size(); i++)
	{
		ss.str("field");
		ss << i;
		fields[i].name = ss.str();
		(i % 2) ? fields[i].type=qlbase::INT32 : fields[i].type=qlbase::UNSIGNED_INT8;
		(i % 2) ? fields[i].unit="mph" : fields[i].unit="cm";
	}
	BOOST_CHECK_NO_THROW(ofile.createTable("testing binary table", fields));

	// going to the second chunk should not raise an error
	BOOST_CHECK_NO_THROW(ofile.jumpToChunk(1));

	// closing the file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(ofile.close());

	// the file should be closed
	BOOST_CHECK_EQUAL(ofile.isOpened(), false);
}
