/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 



Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 

Author: D.W.G. van Kraalingen, 1986

Chapter 13 in documentation WOFOST Version 4.1 (1988)
This routine calculates the gross CO2 assimilation rate of  the whole crop, FGROS, by performing a Gaup.SSIan integration
over depth in the crop canopy. At three different depths in the canopy, i.e. for different values of LAI, the
assimilation rate is computed for given fluxes of photosynthetically active radiation, whereafter integration over depth
takes place. More information on this routine is given by Spitters et al. (1988). The input variables SINB, PARDIR
and PARDif are calculated in routine TOTASS. Subroutines and functions called: none.  Called by routine TOTASS.



C++ implementation

// C++ code by fangh on 6/24/16.
//
// wait to be tested
*/

#include <math.h>
#include <vector>
using namespace std;

//double MAX(double x, double y){
//    return std::max(x, y);
//}


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

    //13.3 three-point Gaup.SSIan integration over LAI
    double FGROS = 0.;

    double LAIC;
    double VISDF;
    double VIST;
    double VISD;
    double VISSHD;
    double FGRSH;
    double VISPP;
    double FGRSUN;
    double FSLLA;
    double FGL;

    for(int i = 0; i < 3; i++){
        LAIC = LAI * XGAUSS[i];
        //absorbed diffuse radiation (VISDF),light from direct origine (VIST) and direct light(VISD)
        VISDF = (1. - REFS) * PARDif * KDif * exp(-KDif * LAIC);
        VIST = (1. - REFS) * PARDIR * KDIRT * exp(-KDIRT * LAIC);
        VISD = (1. - SCV) * PARDIR * KDIRBL * exp(-KDIRBL * LAIC);
        //absorbed flux in W/m2 for shaded leaves and assimilation
        VISSHD = VISDF + VIST - VISD;
        FGRSH = AMAX * (1. - exp(-VISSHD * EFF / max(2.0, AMAX)));
        //direct light absorbed by leaves perpendicular on direct beam and assimilation of sunlit leaf area
        VISPP  = (1. - SCV) * PARDIR / SINB;
        if (VISPP <= 0.) {
            FGRSUN = FGRSH;
        } else {
            FGRSUN = AMAX * (1. - (AMAX - FGRSH) * (1. - exp(-VISPP * EFF / max(2.0, AMAX))) / (EFF * VISPP));
        }
        //fraction of sunlit leaf area (FSLLA) and local assimilation rate (FGL)
        FSLLA = exp(-KDIRBL * LAIC);
        FGL = FSLLA * FGRSUN + (1. - FSLLA) * FGRSH;
        //integration
        FGROS = FGROS + FGL * WGAUSS[i];
    }

    FGROS = FGROS * LAI;
    return FGROS;
}
