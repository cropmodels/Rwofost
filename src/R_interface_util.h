/*
Author: Robert Hijmans
Date: January 2020

License: GNU General Public License (GNU GPL) v. 3
*/


#ifndef R_INTERFACE_UTIL_H_
#define R_INTERFACE_UTIL_H_

#include <Rcpp.h>
using namespace Rcpp;
#include <vector>
#include <string>

template <class T>
T valueFromList(List lst, const char*s) {
	if (!lst.containsElementNamed(s) ) {
		std::string ss = "parameter '" +  std::string(s) + "' not found";
		stop(ss);
	}
	T v = lst[s];
	return(v);
}

template <class T>
T valueFromListDefault(List lst, const char*s, T def) {
	if (!lst.containsElementNamed(s) ) {
		return def;
	}
	T v = lst[s];
	return(v);
}


template <class T>
std::vector<T> vectorFromList(List lst, const char*s) {
	if (!lst.containsElementNamed(s) ) {
		std::string ss = "parameter '" +  std::string(s) + "' not found";
		stop(ss);
	}
	std::vector<T> v = lst[s];
	return(v);
}


template <class T>
std::vector<T> vectorFromDF(DataFrame d, std::string s) {
	CharacterVector nms = d.names();
	auto it = std::find(nms.begin(), nms.end(), s);
	unsigned pos = std::distance(nms.begin(), it);
	if (pos == nms.size()) {
		std::string ss = "Variable '" + s + "' not found";
		stop(ss);
	}
	std::vector<T> v = d[pos];
	return(v);
}


std::vector<double> TableFromList(List lst, const char* s){
	if(! lst.containsElementNamed(s)){
		std::string ss = "parameter '" +  std::string(s) + "' not found";
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

#endif

