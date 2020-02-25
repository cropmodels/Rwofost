/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1.
*/

#include <vector>
#include "SimUtil.h"
#include "wofost.h"



void WofostModel::soil_initialize() {
	ROOTD_initialize();
	if (control.water_limited) {
		if (soil.p.IZT == 0) {
			WATFD_initialize();
		} else {
			WATGW_initialize();
		}
	} else {
		WATPP_initialize();
	}	
}


void WofostModel::soil_rates() {
	ROOTD_rates();
	if (control.water_limited) {
		if (soil.p.IZT == 0) {
			WATFD_rates();
		} else {
			WATGW_rates();
		}
	} else {
		WATPP_rates();
	}
}

void WofostModel::soil_states() {
	ROOTD_states();
	if (control.water_limited) {
		if (soil.p.IZT == 0) {
			WATFD_states();
		} else {
			WATGW_states();
		}
	} else {
		WATPP_states();
	}
}
