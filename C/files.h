#include "date.h"

std::vector<std::string> split(const std::string &s, char delim);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector< std::vector<std::string> > readCSV(std::string);
std::vector<std::vector<std::string> > readINI(const char* filename);
std::vector<std::vector<double> > mFromINI(std::vector<std::vector<std::string> > ini, std::string name);
date dateFromINI(std::vector<std::vector<std::string> > ini, std::string name);
bool bFromINI(std::vector<std::vector<std::string> > ini, std::string name);
int iFromINI(std::vector<std::vector<std::string> > ini, std::string name);
std::vector<double> dvFromINI(std::vector<std::vector<std::string> > ini, std::string name);
std::vector<double> StrVecToDVec(const std::string &s);

//std::string sFromINI(std::vector<std::vector<std::string> > ini, std::string name);
std::string sFromINI(std::vector<std::vector<std::string> > ini, std::string name, std::string def="");
double dFromINI(std::vector<std::vector<std::string> > ini, std::string name, double def=NAN);
