/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 

Author: C.A. van Diepen, September 1988

Compute the depth of the root zone is calculated for each day of the crop cycle
*/

#include "wofost.h"
#include "SimUtil.h"


void WofostModel::ROOTD_initialize() {
    crop.s.RD = crop.p.RDI;
	soil.RDM = std::max(crop.p.RDI, std::min(soil.p.RDMSOL, crop.p.RDMCR));
    if (soil.p.IZT == 0) { 
		soil.ZT = 999.; 
	} else { //   if (soil.p.IZT == 1) 
		soil.ZT = soil.p.ZTI; 
	}
}


void WofostModel::ROOTD_rates() {
// root growth RR in cm 
	crop.r.RR = 0; 

   if (crop.Fr > 0 ) { 
		// with groundwater, root growth zero nearby groundwater
		if (crop.p.IAIRDU || ((soil.ZT - crop.s.RD) >= 10)) { 
			crop.r.RR = std::min(soil.RDM - crop.s.RD, crop.p.RRI);
		}
	} 
}

void WofostModel::ROOTD_states() {
    crop.s.RD += crop.r.RR;
}



