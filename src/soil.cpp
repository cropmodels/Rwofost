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
	soil.EVWMX = atm.E0;
	soil.EVSMX = atm.ES0;

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
