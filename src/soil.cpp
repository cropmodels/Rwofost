/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1.
*/

using namespace std;
#include <vector>
#include "SimUtil.h"
#include "wofost.h"



void WofostModel::soil_initialize() {
	if (control.IWB == 0) {
		WATPP_initialize();
	} else {
		ROOTD_initialize();
		if (soil.p.IZT == 0) {
			WATFD_initialize();
		} else {
			WATGW_initialize();
		}
	}
}


void WofostModel::soil_rates() {
	if (control.IWB == 0) {
		WATPP_rates();
	} else {
		ROOTD_rates();
		if (soil.p.IZT == 0) {
			WATFD_rates();
		} else {
			WATGW_rates();
		}
	}
}

void WofostModel::soil_states() {
	if (control.IWB == 0) {
		WATPP_states();
	} else {
		ROOTD_states();
		if (soil.p.IZT == 0) {
			WATFD_states();
		} else {
			WATGW_states();
		}
	}
}
