/*
Robert Hijmans
July 2016

License: GNU General Public License (GNU GPL) v. 2
*/

#include <vector>
#include "SimUtil.h"
#include "wofost.h"



void WofostModel::soil_initialize() {
	ROOTD_initialize();
	if (control.water_limited) {
		if (soil.p.IZT) {
			WATGW_initialize();
		} else {
			WATFD_initialize();
		}
	} else {
		WATPP_initialize();
	}	
}


void WofostModel::soil_rates() {

	if (control.water_limited) {
		if (soil.p.IZT) {
			WATGW_rates();
		} else {
			WATFD_rates();
		}
	} else {
		WATPP_rates();
	}
}

void WofostModel::soil_states() {
	if (control.water_limited) {
		if (soil.p.IZT) {
			WATGW_states();
		} else {
			WATFD_states();
		}
	} else {
		WATPP_states();
	}
}

