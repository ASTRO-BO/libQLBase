/***************************************************************************
    begin                : Wed Oct 30 2013
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

/**
 * Convert a telemetry FITS to das ddl.
 * Require the keywords CREAT_ID, EXTNAME, TM_ID, TFIELDS.
 */

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cstdlib>
#include<map>
#include "InputFileFITS.h"

using namespace qlbase;
using namespace std;

vector<std::string> names;
vector<std::string> values;
vector<std::string> comments;

string trim(const string& s, const string& delimiter)
{
	string ret(s);

	size_t p = ret.find_first_not_of(delimiter);
	ret.erase(0, p);

	p = ret.find_last_not_of(delimiter);
	if (string::npos != p)
		ret.erase(p+1);

	return ret;
}

std::string getValue(const std::string name)
{
	for(unsigned int i=0; i<names.size(); i++)
		if(names[i].compare(name) == 0)
			return values[i];

	return "";
}

int main(int argc, char* argv[])
{
	if(argc <= 2)
	{
		cout << "\nUsage: ./fits2xml fitsfile xmlfile\n";
		return 0;
	}

	// create type map
	map<char, string> typeMap;
	typeMap['B'] = "byte";
	typeMap['I'] = "int16";
	typeMap['J'] = "int32";
	typeMap['K'] = "int64";
	typeMap['E'] = "float32";
	typeMap['D'] = "float64";

	InputFileFITS infile;
	infile.open(argv[1]);

	ofstream outfile;
	outfile.open(argv[2]);
	outfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
	outfile << "<ddl xmlns=\"http://oats.inaf.it/das\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">" << endl;

	for(unsigned hdunum=0; hdunum < infile.getHeadersNum(); hdunum++)
	{
		// reset global context
		names.resize(0);
		values.resize(0);
		comments.resize(0);

		// move to new header
		infile.moveToHeader(hdunum);

		// read keywords
		int keywordNum = infile.getKeywordNum();
		for(unsigned int i=0; i<keywordNum; i++)
		{
			string keyword = infile.getKeyword(i);
			string::size_type delim1 = keyword.find('=');
			string::size_type delim2 = keyword.find("/");
			std::string name = trim(keyword.substr(0, delim1), " \t\'");
			std::string value = trim(keyword.substr(delim1+1, delim2-delim1-1), " \t\'");
			std::string comment = trim(keyword.substr(delim2+1, keyword.size()), " \t\'");

			names.push_back(name);
			values.push_back(value);
			comments.push_back(comment);
		}

		if(getValue("XTENSION").compare("BINTABLE") == 0)
		{
			// write table types
			outfile << "  <type name=\""+getValue("CREAT_ID")+"_"+getValue("EXTNAME")+"\" description=\""+getValue("TM_ID")+"\">" << endl;
			outfile << "    <data>" << endl;
			outfile << "      <binaryTable>" << endl;
			for(unsigned int i=1; i<=atoi(getValue("TFIELDS").c_str()); i++)
			{
				stringstream stype, sform, sunit;
				string type, form, unit;

				outfile << "        <column";
				stype << "TTYPE" << i;
				type = getValue(stype.str());
				if(type != "")
					outfile << " name=\""+type+"\"";
				sform << "TFORM" << i;
				form = getValue(sform.str());
				if(form != "")
				{
					string t = trim(form, "1234567890");
					outfile << " type=\""+typeMap[t[0]]+"\"";

					string strsize = trim(form, "BIJKED");
					int size = atoi(strsize.c_str());
					if(size > 1)
						outfile << " arraysize=\""+strsize+"\"";
				}
				sunit << "TUNIT" << i;
				unit = getValue(sunit.str());
				if(unit != "")
					outfile << " unit=\""+unit+"\"";
				outfile << " />" << endl;
			}
			outfile << "      </binaryTable>" << endl;
			outfile << "    </data>" << endl;
			outfile << "  </type>" << endl;
		}
	}
	outfile << "</ddl>" << endl;

	infile.close();
	outfile.close();

	return 1;
}
