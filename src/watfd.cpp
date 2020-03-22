/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 


Author: C.A. van Diepen, February 1989, revised July 1990
Modifications by Tamme van der Wal (1997) and Hendrik Boogaard (1998)

In routine WATFD the simulation of the soil water balance is performed for FREELY DRAINING soils
in the water-limited production situation.  WATFD is derived from subroutine APPLE of WOFOST Version 4.1.

The purpose of the soil water balance calculations is to estimate the daily value of the soil moisture content. The soil moisture content influences soil moisture uptake and crop transpiration. In the initial section soil physical data are read from file SOFILE, and all rates and sum totals are set at zero. The dynamic calculations are carried out in two sections, one for the calculation of rates of change per timestep (delt = 1 day) and one for the calculation of summation variables and state variables.
The water balance is driven by rainfall, possibly buffered as surface storage, and evapotranspiration. The processes considered are infiltration, soil water retention, percolation (here conceived as downward water flow from rooted zone to second layer), and the loss of water beyond the maximum root zone.

The textural profile of the soil is conceived as homogeneous. Initially the soil profile consists of two layers, the actually rooted soil and the soil immediately below the rooted zone until the maximum rooting depth (soil and crop dependent). The extension of the root zone
from initial rooting depth to maximum rooting depth is described in subroutine ROOTD. Its effect on the soil moisture content is accounted for in this subroutine WATFD. From the moment that the maximum rooting depth is reached the soil profile is described as a one layer system.
The dynamic output is written to file by subroutine PRIWFD.

VARIABLE TYPE Description                                      Units   I/O
RDM     R*4  maximum rooting depth                              cm      I
RD      R*4  rooting depth                                      cm      I
IAIRDU  I*4  indicates presence(1) or absence(0) of airducts            I
             in the roots. 1= can tolerate waterlogging
IFUNRN  I*4  flag indicating the way to calculate the                   I
             non-infiltrating fraction of rainfall:
             0. fraction is fixed at p.NOTINF
             1. fraction depends on p.NOTINF and on daily rainfall
             as given by p.NINFTB.
SSI     R*4  initial surface storage                            cm      I
SMLIM   R*4  max. initial soil moisture in topsoil              cm      I
SSMAX   R*4  maximum surface storage                            cm      I
WAV     R*4  initial (at emergence) amount of water in excess of        I
             wilting point, but not exceeding field capacity    cm
NOTINF  R*4  if FUNRAI=0 non-infiltrating fraction of rainfall          I
             if FUNRAI=1 maximum non-infiltrating fraction

EVWMX   R*4  maximum evaporation rate from shaded water surface cm d-1  I
EVSMX   R*4  maximum evaporation rate from shaded soil surface  cm d-1  I
TRA     R*4  actual transpiration rate                          cm d-1  I
             Note: TRA is calculated in EVTRA called by CROPSI
SMW     R*4  soil moisture content at wilting point           cm3 cm-3  O
CRAIRC  R*4  critical air content                             cm3 cm-3  O
SM      R*4  actual soil moisture content                     cm3 cm-3  O
RAIN    R*4  daily rainfall                                     cm d-1  I  !!! RH note that unit!
SM0     R*4  soil porosity                                    cm3 cm-3  O
SMFCF   R*4  soil moisture content at field capacity          cm3 cm-3  O
*/


#include <vector>

#include "SimUtil.h"
#include "wofost.h"
#include <math.h>

//int main(){return(0);}


void WofostModel::WATFD_initialize() {
	//old rooting depth
	crop.s.RDOLD = crop.s.RD;
//!! Check whether p.SMLIM is within boundaries (TvdW, 24-jul-97)
	if (soil.p.SMLIM < soil.p.SMW) soil.p.SMLIM = soil.p.SMW;
	if (soil.p.SMLIM > soil.p.SM0) soil.p.SMLIM = soil.p.SM0;
//!! User is NOT notified of changes in p.SMLIM

	soil.ss = soil.p.SSI;
  //  initial moisture content in rooted zone
  //!!  original: p.SMLIM = p.SMFCF
  //!!  now p.SMLIM is input variable in SITE.DAT  (TvdW, 24-jul-97)

	if (crop.p.IAIRDU) soil.p.SMLIM = soil.p.SM0;
	soil.SM = LIMIT (soil.p.SMW, soil.p.SMLIM, soil.p.SMW + soil.p.WAV / crop.s.RD);
// initial amount of moisture in rooted zone
	soil.W  = soil.SM * crop.s.RD;
	soil.WI = soil.W;
// soil evaporation, days since last rain
	soil.DSLR = 1.;
	if (soil.SM <= (soil.p.SMW + 0.5 * (soil.p.SMFCF - soil.p.SMW))) soil.DSLR = 5.;
// amount of moisture between rooted zone and max.rooting depth
  //!!  p.SMLIM replaced by p.SMFCF TvdW 24-jul-97
  //!!  WLOW  = LIMIT (0., p.SMLIM*(p.RDM-RD), p.WAV+p.RDM*p.SMW-W)
	soil.WLOW  = LIMIT (0., soil.p.SM0 * (soil.RDM - crop.s.RD), soil.p.WAV + soil.RDM * soil.p.SMW - soil.W);
	soil.WLOWI =soil.WLOW;
	soil.WWLOW = soil.W + soil.WLOW;

  // summation variables of the water balance are set at zero.
//	soil.EVST   = 0.;
//	soil.EVWT   = 0.;
//	soil.TSR    = 0.;	
//	soil.WDRT   = 0.;
//	soil.PERCT  = 0.;
//	soil.LOSST  = 0.;

//  rates are set at zero
	soil.EVS   = 0.;
	soil.EVW   = 0.;
	soil.RIN   = 0.;
	soil.RINold = 0.;
    soil.RIRR  = 0.;
    soil.DW    = 0.;
    soil.PERC  = 0.;
    soil.LOSS  = 0.;
    soil.DWLOW = 0.;

    // infiltration parameters WOFOST41
    // DATA p.NINFTB/0.0,0.00, 0.5,0.12, 1.0,0.29, 2.0,0.71, 3.0,0.91, 7.0,1.00, 8*0./
    // infiltration parameters WOFOST_WRR
    // this is a multiplier for non-infiltrating fraction of rainfall as a function of daily rainfall
    // RH: this needs to become an input variable?
    soil.p.NINFTB = {0.0, 0.0, 0.5, 0.0, 1.5, 1.0, 0.0, 0.0};
	
}


void WofostModel::WATFD_rates() {
// rates of the water balance

	soil.EVW = 0.;
    soil.EVS = 0.;
// from surface water if surface storage more than 1 cm, ...
    if (soil.ss > 1.)  {
        soil.EVW = soil.EVWMX;
// else from soil surface
    } else {

//!!in old WOFOST:   evs = evsmx * limit(0.,1.,(sm-p.SMW/3.)/(p.SM0-p.SMW/3.))
// RH: looks odd as RIN is a rate that is computed later. Should be based on a state?
// now using RINold as in pcse
		if (soil.RINold >= 1.)  {
			soil.EVS  = soil.EVSMX;
			soil.DSLR = 1.;
		} else {
			soil.DSLR  += 1.;
			double EVSMXT = soil.EVSMX * (sqrt(soil.DSLR) - sqrt(soil.DSLR - 1.));
			soil.EVS = std::min(soil.EVSMX, EVSMXT + soil.RINold);
		}
    }

	double RINPRE;
	if (soil.p.IFUNRN == 0) {
		RINPRE = (1 - soil.p.NOTINF) * atm.RAIN;
	} else {
		RINPRE = (1 - soil.p.NOTINF * AFGEN(soil.p.NINFTB, atm.RAIN)) * atm.RAIN;
	}
    RINPRE += soil.RIRR + soil.ss;
    if (soil.ss > 0.1) {
       //with surface storage, infiltration limited by SOPE
        double AVAIL = RINPRE + soil.RIRR - soil.EVW;
        RINPRE = std::min(soil.p.SOPE, AVAIL);
	}

// percolation
// equilibrium amount of soil moisture in rooted zone
	double WE = soil.p.SMFCF * crop.s.RD;
// percolation from rooted zone to subsoil equals amount of excess moisture in rooted zone (not to exceed conductivity)
	double PERC1 =  LIMIT (0., soil.p.SOPE, (soil.W - WE) - crop.TRA - soil.EVS);

// loss of water at the lower end of the maximum root zone equilibrium amount of soil moisture below rooted zone
	if (!crop.p.IAIRDU) {
		double WELOW = soil.p.SMFCF * (soil.RDM - crop.s.RD);
		soil.LOSS  = LIMIT (0., soil.p.KSUB, (soil.WLOW - WELOW) + PERC1 );
	} else {
// for rice water losses are limited to p.K0/20
		soil.LOSS = std::min (soil.LOSS, 0.05 * soil.p.K0);
	}
// percolation not to exceed uptake capacity of subsoil
	double PERC2 = ((soil.RDM - crop.s.RD) * soil.p.SM0 - soil.WLOW) + soil.LOSS;
	soil.PERC  = std::min (PERC1, PERC2);

// adjustment of infiltration rate
	soil.RIN = std::min(RINPRE, (soil.p.SM0-soil.SM) * crop.s.RD + crop.TRA + soil.EVS + soil.PERC);
	soil.RINold = soil.RIN;
// rates of change in amounts of moisture W and WLOW
	soil.DW    =  soil.RIN - crop.TRA - soil.EVS - soil.PERC;
	soil.DWLOW = soil.PERC - soil.LOSS;
	

    // Check if DW creates a negative value of W
    // If so, reduce EVS to reach W == 0
    double Wtmp = soil.W + soil.DW;
    if (Wtmp < 0.0) {
        soil.EVS += Wtmp;
        soil.DW = -soil.W;
	}
    // Computation of rate of change in surface storage and surface runoff
    // SStmp is the layer of water that cannot infiltrate and that can potentially
    // be stored on the surface. Here we assume that RAIN_NOTINF automatically
    // ends up in the surface storage (and finally runoff).
    //double SStmp = atm.RAIN + soil.RIRR - soil.EVW - soil.RIN;
    // rate of change in surface storage is limited by SSMAX - SS
    //soil.DSS = min(SStmp, (soil.p.SSMAX - soil.ss))
    // Remaining part of SStmp is send to surface runoff
    //soil.DTSR = SStmp - soil.DSS;
	
}


void WofostModel::WATFD_states() {

// total evaporation from surface water layer and/or soil
//	soil.EVWT += soil.EVW;
//	soil.EVST += soil.EVS;

// surface storage and runoff
	double SSPRE = soil.ss + (atm.RAIN + soil.RIRR - soil.EVW - soil.RIN);
	soil.ss    = std::min (SSPRE, soil.p.SSMAX);
//	soil.TSR  += SSPRE - soil.ss;

// amount of water in rooted zone
	soil.W = std::max(0.0, soil.W + soil.DW);
	//if (W_NEW < 0.0)  {
	//	soil.EVST = soil.EVST + W_NEW;
	//	soil.W = 0.0;
	//} else {
	//	soil.W = W_NEW;
	//}

// total percolation and loss of water by deep leaching
	//soil.PERCT += soil.PERC;
	//soil.LOSST += soil.LOSS;

// amount of water in unrooted, lower part of rootable zone
	soil.WLOW += soil.DWLOW;
// total amount of water in the whole rootable zone
	soil.WWLOW = soil.W + soil.WLOW;

//----------------------------------------------
//        change of rootzone subsystem boundary
//----------------------------------------------
// calculation of amount of soil moisture in new rootzone

	if (crop.r.RR > 0.001)  {
// water added to root zone by root growth, in cm
		double WDR  = soil.WLOW * (crop.r.RR)/(soil.RDM - crop.s.RDOLD);
// total water addition to rootzone by root growth
//		soil.WDRT += WDR;
// amount of soil moisture in extended rootzone
		soil.W += WDR;
	}
// mean soil moisture content in rooted zone
	soil.SM = soil.W / crop.s.RD;
  // calculating mean soil moisture content over growing period
   //soil.SUMSM += soil.SM;
  // save rooting depth
	crop.s.RDOLD = crop.s.RD;
}

