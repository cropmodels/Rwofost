/*
Author Robert Hijmans
Date: May 2016

License: GNU General Public License (GNU GPL) v. 2
*/

#ifndef R_INTERFACE_UTIL_H_
#define R_INTERFACE_UTIL_H_

#include <Rcpp.h>
using namespace Rcpp;
#include <vector>
#include <string>


inline std::vector<double> doubleFromDF(DataFrame d, const char* s) {
	CharacterVector nms = d.names();
	for (int i=0; i < nms.size(); i++) {
		if (nms[i] == s) {
			std::vector<double> v = d[i];
			return(v);
		}
	}
    std::string ss = "Variable '" +  std::string(s) + "' not found";
    stop(ss);
	// to avoid "warning: control reaches end of non-void function"
	std::vector<double> v(1);
	return( v );
}


inline std::vector<long> longFromDF(Rcpp::DataFrame d, const char* s) {
	Rcpp::CharacterVector nms = d.names();
	for (int i=0; i < nms.size(); i++) {
		if (nms[i] == s) {
			std::vector<long> v = d[i];
			return(v);
		}     
	}
    std::string ss = "Variable '" +  std::string(s) + "' not found";
    stop(ss);  
	// to avoid "warning: control reaches end of non-void function"
	std::vector<long> v;
	return( v );
}


inline DateVector dateFromDF(DataFrame d, const char* s) {

//	int nrow = d.nrows();
	CharacterVector nms = d.names();
	for (int i=0; i < nms.size(); i++) {
		if (nms[i] == s) {
			DateVector v = d[i];
			return(v);
		}
	}
    std::string ss = "Variable '" +  std::string(s) + "' not found";
    stop(ss);
	// to avoid "warning: control reaches end of non-void function"
	DateVector v(1);
	return( v );
}


inline std::vector<double> vecFromMat(NumericMatrix m, const char* s) {

 	CharacterVector nms = colnames(m);

	for (int j=0; j < nms.size(); j++) {
		if (nms[j] == s) {
			int nrow = m.nrow();
			std::vector<double> v(nrow);
			for (int i = 0; i < nrow; i++) {
				v[i] = m(i,j);
			}
			return(v);
		}
	}
	std::string ss = "Variable '" +  std::string(s) + "' not found";
	stop(ss);
	// to avoid "warning: control reaches end of non-void function"
	std::vector<double> v(1);
	return( v );

}


inline double doubleFromList(List lst, const char* s) {
	if (!lst.containsElementNamed(s) ) {
		std::string ss = "parameter '" +  std::string(s) + "' not found";
		stop(ss);
	}
// todo: check if this is a single number ?
	double v = lst[s];
	return(v);
}


inline int intFromList(List lst, const char* s) {
	if (!lst.containsElementNamed(s) ) {
		std::string ss = "parameter '" +  std::string(s) + "' not found";
		stop(ss);
	}
// todo: check if this is a single number ?
	int v = lst[s];
	return(v);
}


inline int OptionalintFromList(List lst, const char* s) {
	if (!lst.containsElementNamed(s) ) {
		return(0);
	}
// todo: check if this is a single number ?
	int v = lst[s];
	return(v);
}

inline bool boolFromList(List lst, const char* s) {
	if (!lst.containsElementNamed(s) ) {
		std::string ss = "parameter '" +  std::string(s) + "' not found";
		stop(ss);
	}
// todo: check if this is a single number ?
	bool v = lst[s];
	return(v);
}

inline DateVector datesFromList(List lst, const char* s) {
  if (!lst.containsElementNamed(s)) {
    // todo: check if this is a single number
    std::string ss = "parameter '" +  std::string(s) + "' not found";
    stop(ss);
  }
  DateVector v = lst[s];
  return(v);
}


inline std::vector<long> longFromList(List lst, const char* s) {
	if (!lst.containsElementNamed(s) ) {
		std::string ss = "parameter '" +  std::string(s) + "' not found";
		stop(ss);
	}
// todo: check if this is a single number ?
	std::vector<long> v = lst[s];
	return(v);
}

inline std::vector<std::vector<double> > matFromList(List lst, const char* s) {

	if (! lst.containsElementNamed(s)) {
		std::string ss = "parameter matrix '" +  std::string(s) + "' not found";
		stop(ss);
	}

// todo: check if this is a matrix
	NumericMatrix x = lst[s];
	int nrow = x.nrow();
	std::vector<std::vector<double> > m(nrow, std::vector<double>(2));
	for (int i=0; i < nrow; i++) {
		m[i][0] = x(i,0);
		m[i][1] = x(i,1);
	}
	return(m);
}


inline std::vector<double> TBFromList(List lst, const char* s){
	if(! lst.containsElementNamed(s)){
		std::string ss = "parameter matrix '" +  std::string(s) + "' not found";
		stop(ss);
	}

	NumericMatrix x = lst[s];
	if(x.ncol() != 2){
		std::string ss2 = "ncol != 2";
		stop(ss2);
	}
	int nrow = x.nrow();
	std::vector<double> result;
	for(int i = 0; i < nrow; i++){
		result.push_back( x(i, 0) );
		result.push_back( x(i, 1) );
	}
	return result;
}

inline std::vector<double> TBFromList2(List lst, const char* s){
	if(! lst.containsElementNamed(s)){
		std::string ss = "parameter matrix '" +  std::string(s) + "' not found";
		stop(ss);
	}

	NumericMatrix x = lst[s];
	if(x.nrow() != 2){
		std::string ss2 = "nrow != 2";
		stop(ss2);
	}
	//std::vector<double> result;
	//result.insert(result.end(), x.begin(), x.end());
	std::vector<double> result = Rcpp::as<std::vector<double> >(x);

	return result;
}


inline std::vector<double> oldTBFromList2(List lst, const char* s){
	if(! lst.containsElementNamed(s)){
		std::string ss = "parameter matrix '" +  std::string(s) + "' not found";
		stop(ss);
	}

	NumericMatrix x = lst[s];
	if(x.ncol() != 2){
		std::string ss2 = "ncol != 2";
		stop(ss2);
	}
	size_t nrow = x.nrow();
	std::vector<double> result;
	for(size_t i = 0; i < 2; i++){
		for(size_t j = 0; j < nrow; j++){
			result.push_back( x(j, i) );
		}
	}
	return result;
}


inline std::vector<double> vecFromList(List lst, const char* s){
	if(! lst.containsElementNamed(s)){
		std::string ss = "parameter matrix '" +  std::string(s) + "' not found";
		stop(ss);
	}

	NumericVector x = lst[s];
	int len = x.size();
	std::vector<double> result;
	for(int i = 0; i < len; i++){
		result.push_back( x(i) );
	}
	return result;
}

#endif