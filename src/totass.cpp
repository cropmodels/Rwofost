/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 

Author: D.W.G. van Kraalingen, 1986

Chapter 13 in documentation WOFOST Version 4.1 (1988)
This routine calculates the gross CO2 assimilation rate of  the whole crop, FGROS, by performing a Gaussian integration over depth in the crop canopy. At three different depths in the canopy, i.e. for different values of LAI, the assimilation rate is computed for given fluxes of photosynthetically active radiation, whereafter integration over depth takes place. More information on this routine is given by Spitters et al. (1988). The input variables SINB, PARDIR
and PARDif are calculated in routine TOTASS. Subroutines and functions called: none.  Called by TOTASS.
*/

#include <math.h>
#include <algorithm>
#include "wofost.h"


double ASSIM(double AMAX, double EFF, double LAI, double KDif, double SINB, double PARDIR, double PARDif) {
    //13.1 initialize GAUSS array and scattering coefficient
    double XGAUSS[3] = {0.1127017, 0.5000000, 0.8872983};
    double WGAUSS[3] = {0.2777778, 0.4444444, 0.2777778};
    double SCV = 0.2;

    //13.2 extinction coefficients KDif,KDIRBL,KDIRT
    double REFH = (1 - sqrt(1. - SCV)) / (1 + sqrt(1. - SCV));
    double REFS = REFH * 2. / (1. + 1.6 * SINB);
    double KDIRBL = (0.5 / SINB) * KDif / (0.8 * sqrt(1. - SCV));
    double KDIRT = KDIRBL * sqrt(1. - SCV);

    //13.3 three-point Gaupssian integration over LAI
    double FGROS = 0.;


    for(int i = 0; i < 3; i++){
        double LAIC = LAI * XGAUSS[i];
        //absorbed diffuse radiation (VISDF),light from direct origine (VIST) and direct light(VISD)
        double VISDF = (1. - REFS) * PARDif * KDif * exp(-KDif * LAIC);
        double VIST = (1. - REFS) * PARDIR * KDIRT * exp(-KDIRT * LAIC);
        double VISD = (1. - SCV) * PARDIR * KDIRBL * exp(-KDIRBL * LAIC);
        //absorbed flux in W/m2 for shaded leaves and assimilation
        double VISSHD = VISDF + VIST - VISD;
        double FGRSH = AMAX * (1. - exp(-VISSHD * EFF / std::max(2.0, AMAX)));
        //direct light absorbed by leaves perpendicular on direct beam and assimilation of sunlit leaf area
        double VISPP  = (1. - SCV) * PARDIR / SINB;
		double FGRSUN;
        if (VISPP <= 0.) {
            FGRSUN = FGRSH;
        } else {
            FGRSUN = AMAX * (1. - (AMAX - FGRSH) * (1. - exp(-VISPP * EFF / std::max(2.0, AMAX))) / (EFF * VISPP));
        }
        //fraction of sunlit leaf area (FSLLA) and local assimilation rate (FGL)
        double FSLLA = exp(-KDIRBL * LAIC);
        double FGL = FSLLA * FGRSUN + (1. - FSLLA) * FGRSH;
        //integration
        FGROS = FGROS + FGL * WGAUSS[i];
    }

    FGROS = FGROS * LAI;
    return FGROS;
}

/*
TOTASS 

Authors: Daniel van Kraalingen, April 1991

Purpose: This routine calculates the daily total gross CO2 assimilation by performing a Gaussian integration over time. At three different times of the day, irradiance is computed and used to calculate the instantaneous canopy assimilation, whereafter integration takes place. More information on this routine is given by Spitters et al. (1988).

FORMAL PARAMETERS:  (I=input,O=output)
name   type meaning                                    units  class
----   ---- -------                                    -----  -----
DAYL    R4  Astronomical daylength (base = 0 degrees)     h      I
AMAX    R4  Assimilation rate at light saturation      kg CO2/   I
                                                      ha leaf/h
EFF     R4  Initial light use efficiency              kg CO2/J/  I
                                                      ha/h m2 s
LAI     R4  Leaf area index                             ha/ha    I
KDif    R4  Extinction coefficient for diffuse light             I
AVRAD   R4  Daily shortwave radiation                  J m-2 d-1 I
DifPP   R4  Diffuse irradiation perpendicular to direction of
            light                                      J m-2 s-1 I
DSINBE  R4  Daily total of effective solar height         s      I
SINLD   R4  Seasonal offset of sine of solar height       -      I
COSLD   R4  Amplitude of sine of solar height             -      I
DTGA    R4  Daily total gross assimilation           kg CO2/ha/d O

*/

//double DAYL, double AMAX, double EFF, double LAI, double KDif, double AVRAD, double SINLD, double COSLD, double DSINBE, double DifPP)

double WofostModel::TOTASS() {

    //Gauss points and weights are stored in an array
    double XGAUSS[3] = {0.1127017, 0.5000000, 0.8872983};
    double WGAUSS[3] = {0.2777778, 0.4444444, 0.2777778};

    double DTGA = 0;
	double PI = 3.141592653589793238462643383279502884197169399375;


    if( crop.AMAX > 0. && crop.s.LAI > 0) {
        for (int i = 0; i < 3; i++) {
            double HOUR = 12.0 + 0.5 * atm.DAYL * XGAUSS[i];
            double SINB = std::max(0., atm.SINLD + atm.COSLD * cos(2 * PI * (HOUR + 12) / 24));
            double PAR = 0.5 * atm.AVRAD * SINB * (1.+0.4 * SINB) / atm.DSINBE;
            double PARDIF = std::min(PAR, SINB * atm.DifPP);
            double PARDIR = PAR-PARDIF;
            double FGROS = ASSIM (crop.AMAX, crop.EFF, crop.s.LAI, crop.KDif, SINB, PARDIR, PARDIF);
            DTGA = DTGA + FGROS * WGAUSS[i];

        }
        DTGA = DTGA * atm.DAYL;

    }
    return DTGA;
}
