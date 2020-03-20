/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 

Author : Kees van Diepen, April 1989, revised August 1990


Determine the sowing date

For ISTCHO = 2  a suitable sowing date is calculated, and the corresponding date of emergence.
First the workability of the soil over the period before sowing is checked on the basis of soil
and crop dependent criteria. The workability model imitates the results of the hydraulic model of
WIND (1972), published in ICW Nota 867 (VAN WIJK AND FEDDES,1975). Soil EC-3 in the present model
corresponds with Wind's loam with a drain  depth of 150 cm.

 //-----------------------------------------------------------------------
 CAPRMX     maximum upward flow into plow layer
 CAPRFU(10) upward flow as a function of negative values of WEXC, when
             topsoil is drier than field capacity
 COSUT      counts times that sowing date equals latest sowing date
             (indicator for the suitability of a soil for a specific crop).
 p.DEFLIM     minimum required soil moisture deficit in plow layer
             for occurrence of workable day (workability criterion)
 EVS        daily evaporation from bare soil surface
 DOY       julian date
 IDFWOR     first workable day
 IDESOW     earliest sowing date
 IDLSOW     latest sowing date
 IDSOW      sowing date
 ILWPER     length of workable period, should be 3 for sowing
            (sowing criterion)
 RAIN       daily rainfall
 SEEP       daily seepage from plow layer
 SMDEF      estimated soil moisture deficit in plow layer
 SPADS      SPAC  topsoil seepage parameter for deep seedbed (potato)
 SPASS      SPAC  topsoil seepage parameter for shallow seedbed
 SPODS      SPOC  topsoil seepage parameter for deep seedbed (potato)
 SPOSS      SPOC  topsoil seepage parameter for shallow seedbed
 WEXC       excess amount of water in plow layer

*/


#include "wofost.h"
#include "SimUtil.h"

void WofostModel::STDAY_initialize() {
	
// RH: this is a strange hack
/*    if( soil.CRPNAM.substr(1,2) == "BI" ){ 
// two seepage parameters for deep seedbed (potato)
        soil.SPAC = soil.p.SPADS;
        soil.SPOC = soil.p.SPODS;
    }
    else{
*/

	// 1st topsoil seepage parameter deep seedbed
	soil.p.SPADS = 0.800;
	// 2nd topsoil seepage parameter deep seedbed
	soil.p.SPODS = 0.040;
	// 1st topsoil seepage parameter shallow seedbed
	soil.p.SPASS = 0.900;
	// 2nd topsoil seepage parameter shallow seedbed
	soil.p.SPOSS = 0.070;
	// required moisture deficit deep seedbed
	soil.p.DEFLIM = 0.000;
		
	control.IDESOW = 0;
	control.IDLSOW = 15;
		
// two seepage parameters for shallow seedbed (all other crops)
    soil.CAPRFU = {-0.50,0.50, 0.00,0.20, 0.10,0.15, 0.40,0.10, 1.00,0.05};
    soil.SPAC = soil.p.SPASS;
    soil.SPOC = soil.p.SPOSS;
    soil.p.DEFLIM = 0.;

    soil.WEXC = 2.;
    //RAIN=0?
//         RAIN  = 0.
    soil.EVS = 0.;
    soil.CAPRMX = 0.;
    soil.SEEP = 0.;
    soil.IDFWOR = -99;
    soil.ILWPER = 0;
}


void WofostModel::STDAY() {
// workability is assessed until sowing date is found

    // evaporation from soil surface
    if (soil.WEXC >= 0.5){
        soil.CAPRMX = 0.;
        soil.EVS = atm.ES0;
    } else { 
    //  maximum capillary rise to surface (cf. Ritchie)
        soil.CAPRMX = AFGEN(soil.CAPRFU, -soil.WEXC);
        soil.EVS = std::min( atm.ES0, soil.CAPRMX + atm.RAIN );
    }
    soil.WEXC = std::max(-1., soil.WEXC + atm.RAIN - soil.EVS);
    // seepage
    if (soil.WEXC > 0.){
        soil.SEEP = std::min(soil.WEXC * soil.SPAC + soil.SPOC, soil.WEXC);
        soil.WEXC = soil.WEXC - soil.SEEP;
    }
    // criterion for workable day
    if (soil.WEXC <= soil.p.DEFLIM ){
        soil.ILWPER = soil.ILWPER + 1;
    } else {
        soil.ILWPER = 0;
    }

    // first workable day? (for output only)
//    if (soil.IDFWOR == -99 && soil.ILWPER >= 1) {
//        soil.IDFWOR = day;
//    }
    // criterion for sowing
    if ((step >= control.IDESOW && soil.ILWPER >= 3) || step == control.IDESOW){
        ISTATE = 1;
        // indicator for sowing at latest sowing date
        if (DOY == control.IDESOW){
            soil.COSUT = 1.;
        } else {
            soil.COSUT = 0.;
        }
	}
}





