/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1.

Author: C.A. van Diepen, April 1989

EVTRA routine calculates for a given crop cover the maximum evaporation rate from a shaded wet soil surface and from a shaded water surface, and the maximum and actual crop transpiration rate.

SWEAF computes the fraction of easily available soil water between field capacity and wilting point is a function of the potential evapotranspiration rate (for a closed canopy) in cm/day, ET0, and the crop group number, CGNR (from 1 (=drought-sensitive) to 5 (=drought-resistant)). The function SWEAF describes this relationship given in tabular form by Doorenbos and  Kassam (1979) and by Van Keulen and Wolf (1986; p.108, table 20). Chapter 20 in documentation WOFOST Version 4.1 (1988)


*/

#include <vector>
#include <math.h>
#include "wofost.h"
#include "SimUtil.h"


double SWEAF(double ET0, double CGNR){
    double A = 0.76, B = 1.5;
    double sweaf = 1./(A + B * ET0) - (5. - CGNR) * 0.10;
    if (CGNR < 3.) {
        sweaf = sweaf + (ET0 - 0.6) / (CGNR * (CGNR + 3.));
    }
    sweaf = LIMIT (0.10, 0.95, sweaf);
    return sweaf;
}



void WofostModel::EVTRA() {

// bool IOXWL, int p.IAIRDU, double KDif, double p.CFET, double p.DEPNR,
//        double E0, double ES0, double ET0, double LAI, double SM, double p.SM0, double p.SMFCF, double p.SMW ,double p.CRAIRC){

    //extinction coefficient for total global radiation
    double KGLOB = 0.75 * crop.KDif;
    //crop specific correction on potential transpiration rate
    atm.ET0 = crop.p.CFET * atm.ET0;
              
    //maximum evaporation and transpiration rates
    double EKL = exp( -KGLOB * crop.s.LAI);
    soil.EVWMX = atm.E0 * EKL;
    soil.EVSMX = std::max(0., atm.ES0 * EKL);
    crop.TRAMX = std::max(0.0001, atm.ET0*(1. - EKL));

     //actual transpiration rate
    if (! control.water_limited) {
        crop.TRA  = crop.TRAMX;
    } else {
        //calculation critical soil moisture content
        double SWDEP = SWEAF(atm.ET0, crop.p.DEPNR);
        double SMCR = (1. - SWDEP) * (soil.p.SMFCF - soil.p.SMW) + soil.p.SMW;
        //reduction in transpiration in case of water shortage
        double RFWS = LIMIT(0.,1., (soil.SM - soil.p.SMW) / (SMCR - soil.p.SMW));

        //reduction in transpiration in case of oxygen shortage
        //for non-rice crops and possibly deficient land drainage        
	    double RFOS = 1.;
		if ((!crop.p.IAIRDU) && (control.IOXWL)){
            //critical soil moisture content for aeration
            double SMAIR = soil.p.SM0 - soil.p.CRAIRC;
            //count days since start oxygen shortage (up to 4 days)
		    double DSOS = 0;
            if (soil.SM >= SMAIR){
                DSOS = std::min((DSOS + 1.), 4.);
            } 
            //maximum reduction reached after 4 days
            double RFOSMX = clamp(0, 1, (soil.p.SM0 - soil.SM)/(soil.p.SM0 - SMAIR));
			RFOS   = RFOSMX + (1 - DSOS/4) * (1-RFOSMX);
        } 	
		crop.RFTRA =  RFWS * RFOS;
        crop.TRA =  crop.RFTRA * crop.TRAMX;
		crop.TRANRF = crop.TRA / crop.TRAMX;		
    }
}
