#include<IO/InputFileFITS.h>
#include<IO/OutputFileFITS.h>
#include<sstream>
#include<fstream>
#include<iomanip>
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
	BOOST_CHECK_NO_THROW(file.open("sample.fits"));

	// jumping on a bad chunck should raise an exception
	BOOST_CHECK_THROW(file.jumpToChunk(10), qlbase::IOException);

	// jumping on an existing chunck shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.jumpToChunk(1));

	// reading the first 4 rows from column 0 shouldn't raise an exception
	std::vector<int32_t> rowsT1;
	BOOST_CHECK_NO_THROW(rowsT1 = file.read32i(0, 0, 3));

	// and their values should be n. 1, 2, 3, 4
	std::vector<int64_t> expectedT1(4);
	expectedT1[0] = 0;
	expectedT1[1] = 1;
	expectedT1[2] = 2;
	expectedT1[3] = 3;
	BOOST_CHECK_EQUAL_COLLECTIONS(&rowsT1[0], &rowsT1[rowsT1.size()], &expectedT1[0], &expectedT1[expectedT1.size()]);

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
	BOOST_CHECK_EQUAL_COLLECTIONS(&rowsT2[0], &rowsT2[rowsT2.size()], &expectedT2[0], &expectedT2[expectedT2.size()]);

	// the file should be open
	BOOST_CHECK_EQUAL(file.isOpened(), true);

	// jumping on an existing chunck shouldn't raise an exception
	BOOST_CHECK_NO_THROW(file.jumpToChunk(2));

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
		ss.str("");
		ss << "field" << i;
		fields[i].name = ss.str();
		(i % 2 == 0) ? fields[i].type=qlbase::INT32 : fields[i].type=qlbase::UNSIGNED_INT8;
		(i % 2 == 0) ? fields[i].unit="mph" : fields[i].unit="cm";
	}
	BOOST_CHECK_NO_THROW(ofile.createTable("testing binary table", fields));

	// going to the second chunk should not raise an error
	BOOST_CHECK_NO_THROW(ofile.jumpToChunk(1));

	// closing the file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(ofile.close());

	// the file should be closed
	BOOST_CHECK_EQUAL(ofile.isOpened(), false);

	// open the fits file should not raise an exception
	BOOST_CHECK_NO_THROW(ofile.open("testing.fits"));

	// going to the second chunk should not raise an exception
	BOOST_CHECK_NO_THROW(ofile.jumpToChunk(1));

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

	// closing the file shouldn't raise an exception
	BOOST_CHECK_NO_THROW(ofile.close());

	// TODO clean (remove testing.fits file)
}
