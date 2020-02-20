#include <iostream>
#include <string>
#include <vector>
#include <fstream>      // std::ifstream
#include <sstream>
#include <algorithm>
#include "files.h"
#include "date.h"

using namespace std;

//Elizabeth Card
// http://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
bool fileExists(string fileName) {
	ifstream test;
	test.open(fileName.c_str());
	if (test.fail()) {
		test.close();
		return false;
	} else {
		test.close();
		return true;
	}
}



string getFileExtension(string filename) {
	int period = 0;
	for (unsigned int i = 0; i < filename.length(); i++) {
		if (filename[i] == '.') {
			period = i;
		}
	}

	string extension;

	for (unsigned int i = period; i < filename.length(); i++) {
		extension += filename[i];
	}

	return(extension);
}




// Jim M.
// http://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
std::vector< std::vector<std::string> > readCSV(std::string filename ) {
    std::ifstream file( filename.c_str() );
    std::vector< std::vector<std::string> > matrix;
    std::vector<std::string>   row;
    std::string line;
    std::string cell;

    while( file ) {
        std::getline(file,line);
        std::stringstream lineStream(line);
        row.clear();

        while( std::getline( lineStream, cell, ',' ) )
            row.push_back( cell );

        if( !row.empty() )
            matrix.push_back( row );
    }

	return(matrix);
}



/*
int main() {
    std::string fname = "test.csv";
	std::vector< std::vector<std::string> >  matrix;
	matrix = readCSV(fname);

	for( int i=0; i<int(matrix.size()); i++ ) {
        for( int j=0; j<int(matrix[i].size()); j++ )
            std::cout << matrix[i][j] << " ";
        std::cout << std::endl;
    }
	return(0);
}
*/




std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
    	// remove spaces between equal sign and name
    	item.erase(std::remove(item.begin(), item.end(), ' '), item.end());
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}


std::vector<std::string> readINIfile(const char* filename) {
	std::ifstream file(filename);
    if(file.fail()){
		std::cout << filename << " not found" << std::endl;
	}

	std::string str;
	std::vector<std::string> lines;

	while (std::getline(file, str)) {
		lines.push_back(str);
	}
	return(lines);
}



std::vector<std::vector<std::string> > readINI(const char* filename) {
	std::ifstream file(filename);
    if(file.fail()){
		std::cout << filename << " not found" << std::endl;
	}
	std::string str;
	std::vector<std::string> s;
	while (std::getline(file, str)) {
		s.push_back(str);
	}

	std::vector<std::string> ss;
	std::vector<std::vector<std::string> > ini(2, std::vector<std::string>(s.size()));

	int k = 0;
	int size = s.size();
	for (int i=0; i < size; i++){
		//skips comments and lines starting with brackets (i.e. section dividers)
		if( s[i][0] == '#' || s[i][0] == '[') continue;
		ss = split(s[i], '=');
		if (ss.size() <= 1) continue;
		ini[0][k] = ss[0];
		ini[1][k] = ss[1];
		k++;
	}
	ini[0].resize(k);
	ini[1].resize(k);
	return(ini);
}

std::vector<std::vector<double> > StrVecToMatrix(const std::string &s) {
	std::vector<std::string> ss = split(s, ',');
	int nrow= ss.size() / 2;
	std::vector<std::vector<double> > m (nrow, std::vector<double>(2));
	int j = 0;
	for (int i=0; i < nrow; i ++) {
		//std::string a = ss[j];
		m[i][0] = strtod(ss[j].c_str(), NULL);
		m[i][1] = strtod(ss[j+1].c_str(), NULL);
		j = j + 2;
	}
	return(m);
}

std::vector<double> StrVecToDVec(const std::string &s) {
    std::vector<std::string> ss = split(s, ',');
    int nrow= ss.size();
    std::vector<double> m;
    for (int i = 0; i < nrow; i ++) {
        m.push_back(strtod(ss[i].c_str(), NULL));
    }
    return(m);
}

std::vector<std::string> getINIvalues(const char* filename, std::vector<std::string> ininames) {

	std::vector<std::vector<std::string> > ini = readINI(filename);
	std::vector<std::string> names = ini[0];
	std::vector<std::string> values = ini[1];
	std::vector<std::string> out(ininames.size());
	int size = names.size();
	int loopsize = 	ininames.size();
	for (int i=0; i < loopsize; i++) {
		//std::string("LAIi")
		int p = find(names.begin(), names.end(), ininames[i]) - names.begin();
		if ( p < size) {
			out[i] = values[p];
		} else {
			out[i] = "-9999";
		}
	}
	return(out);
}

double dFromINI(std::vector<std::vector<std::string> > ini, std::string name) {
	double out;
	int p = find(ini[0].begin(), ini[0].end(), name) - ini[0].begin();

	int inisize = ini[0].size();
	if ( p < inisize) {
		out = strtod(ini[1][p].c_str(), NULL);
	} else {
		std::cout << "missing parameter: " << name << std::endl;
        //exit(1);
	}
	return(out);
}

std::vector<std::vector<double> > mFromINI(std::vector<std::vector<std::string> > ini, std::string name) {
	std::vector<std::vector<double> > out;
	int p = find(ini[0].begin(), ini[0].end(), name) - ini[0].begin();
	int inisize = ini[0].size();
	if ( p < inisize) {
		out = StrVecToMatrix(ini[1][p]);
	} else {
		std::cout << "missing parameter: " << name << std::endl;
        //exit(1);
	}
	return(out);
}

date dateFromINI(std::vector<std::vector<std::string> > ini, std::string name) {
	//double out;
	int p = find(ini[0].begin(), ini[0].end(), name) - ini[0].begin();
	std::vector<std::string> ss;
	date dates;

	int inisize = ini[0].size();
	if ( p < inisize) {
		ss = split(ini[1][p], '-');
		dates.set_year( stoi(ss[0].c_str()) );
		dates.set_month( stoi(ss[1].c_str()) );
		dates.set_day( stoi(ss[2].c_str()) );

	} else {
		std::cout << "missing parameter: " << name << std::endl;
        //exit(1);
	}
	return(dates);
}

bool bFromINI(std::vector<std::vector<std::string> > ini, std::string name) {
	bool out;
	int p = find(ini[0].begin(), ini[0].end(), name) - ini[0].begin();

	int inisize = ini[0].size();
	if ( p < inisize) {
		if(ini[1][p] == "true")
			out = true;
		else
			out = false;
	} else {
		std::cout << "missing parameter: " << name << std::endl;
        //exit(1);
	}
	return(out);
}

string sFromINI(std::vector<std::vector<std::string> > ini, std::string name) {
	string out;
	int p = find(ini[0].begin(), ini[0].end(), name) - ini[0].begin();

	int inisize = ini[0].size();
	if ( p < inisize) {
		out = ini[1][p].c_str();
	} else {
		std::cout << "missing file name: " << name << std::endl;
        //exit(1);
	}
	return(out);
}

int iFromINI(std::vector<std::vector<std::string> > ini, std::string name) {
	int out;
	int p = find(ini[0].begin(), ini[0].end(), name) - ini[0].begin();

	int inisize = ini[0].size();
	if ( p < inisize) {
		out = std::stoi( ini[1][p].c_str() );
	} else {
		std::cout << "missing parameter: " << name << std::endl;
        //exit(1);
	}
	return(out);
}

std::vector<double> dvFromINI(std::vector<std::vector<std::string> > ini, std::string name) {
    std::vector<double> out;
    int p = find(ini[0].begin(), ini[0].end(), name) - ini[0].begin();
    int inisize = ini[0].size();
    if ( p < inisize) {
        out = StrVecToDVec(ini[1][p]);
    } else {
        std::cout << "missing parameter: " << name << std::endl;
        //exit(1);
    }
    return(out);
}
