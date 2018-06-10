/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 

Author: C.A. van Diepen, September 1988

In this routine the depth of the root zone is calculated for each day of the crop cycle
*/

#include "wofost.h"
#include "SimUtil.h"
#include <iostream>
using namespace std;

void WofostModel::ROOTD_initialize() {
// compute the depth of the root zone for each day of the crop cycle	
    crop.RD = crop.p.RDI;
    if(control.IWB == 0){
        soil.p.RDM = max(crop.p.RDI, crop.p.RDMCR);
        //crop.p.RDMO = 0.;
    } else { // if(control.IWB == 1){
        //soil.p.RDM = max(crop.p.RDI, min(min(soil.p.RDMSOL, crop.p.RDMO), crop.p.RDMCR));
		soil.p.RDM = max(crop.p.RDI, min(soil.p.RDMSOL, crop.p.RDMCR));
        //test
        //cout << "p.RDMSOL: " << soil.p.RDMSOL << " p.RDMO: " << crop.p.RDMO << " RMDCR: " << crop.p.RDMCR << endl;
    }
    if (soil.p.IZT == 0) { 
		soil.ZT = 999.; 
	} else { //   if (soil.p.IZT == 1) 
		soil.ZT = soil.p.ZTI; 
	}
}


void WofostModel::ROOTD_rates() {
// compute the depth of the root zone for each day of the crop cycle

    //        root growth RR in cm (is not considered as a rate!)
    crop.RR = min(soil.p.RDM - crop.RD, crop.p.RRI * DELT);
    if(crop.FR <= 0.) { crop.RR = 0.; }
    //        with groundwater, root growth zero nearby groundwater
    if(crop.p.IAIRDU == 0 && soil.ZT - crop.RD < 10.) { crop.RR = 0.; }
}

void WofostModel::ROOTD_states() {
// compute the depth of the root zone for each day of the crop cycle	
    crop.RD = crop.RD + crop.RR;
}



