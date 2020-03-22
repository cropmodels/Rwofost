/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1.

Subroutine WATPP is derived from subroutine APPLE of WOFOST Version 4.1. WATPP is called by WOFSIM. In this routine the variables of the soil water balance in the potential production situation are calculated. The purpose is to quantify the crop water requirements for continuous growth without drought stress. It is assumed that the soil is permanently at field capacity.

Author: C.A. van Diepen, February 1989, revised July 1990


VARIABLE TYPE Description                                      Units   I/O

  p.IAIRDU  I*4  indicates presence(1) or absence(0) of airducts            I
               in the roots. 1= can tolerate waterlogging
  p.SM0     R*4  soil porosity                                    cm3 cm-3  O
  p.SMFCF   R*4  soil moisture content at field capacity          cm3 cm-3  O
  p.SMW     R*4  soil moisture content at wilting point           cm3 cm-3  O
  EVWMX   R*4  maximum evaporation rate from shaded water surface cm d-1  I
  EVSMX   R*4  maximum evaporation rate from shaded soil surface  cm d-1  I
  TRA     R*4  actual transpiration rate                          cm d-1  I
               Note: TRA is calculated in EVTRA called by CROPSI
  SM      R*4  actual soil moisture content                     cm3 cm-3  O
*/

#include "wofost.h"

void WofostModel::WATPP_initialize() {
    soil.SM   = soil.p.SMFCF;
//    soil.EVST = 0.;
//    soil.EVWT = 0.;
    soil.EVS  = 0.;
    soil.EVW  = 0.;
}


void WofostModel::WATPP_rates() {
// rates of the water balance for potential production

// evaporation rate from soil EVS (for non-rice crops) or water surface EVW (for rice)
    if (!crop.p.IAIRDU) {
		soil.EVS = soil.EVSMX * (soil.p.SMFCF - soil.p.SMW / 3.) / (soil.p.SM0 - soil.p.SMW / 3.);
		soil.EVW  = 0;
    } else {
		soil.EVS = 0;
		soil.EVW = soil.EVWMX;
    }
}


void WofostModel::WATPP_states() {
// states of the water balance for potential production

// total evaporation from surface water layer and/or soil
  //       soil.EVWT = soil.EVWT + soil.EVW;
  //       soil.EVST = soil.EVST + soil.EVS;
// soil permanently at field capacity under potential production :
         soil.SM  = soil.p.SMFCF;
}
