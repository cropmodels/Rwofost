/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 

Authors: Daniel van Kraalingen, April 1991

Purpose: This routine calculates the daily total gross CO2 assimilation by performing a Gaup.SSIan integration over
time. At three different times of the day, irradiance is computed and used to calculate the instantaneous canopy
assimilation, whereafter integration takes place. More information on this routine is given by Spitters et al. (1988).

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

#include <math.h>
#include <vector>
#include "wofost.h"

double TOTASS(double DAYL, double AMAX, double EFF, double LAI, double KDif, double AVRAD, double SINLD, double COSLD, double DSINBE, double DifPP){

    //Gauss points and weights are stored in an array
    double XGAUSS[3] = {0.1127017, 0.5000000, 0.8872983};
    double WGAUSS[3] = {0.2777778, 0.4444444, 0.2777778};

    double DTGA = 0;
    double HOUR;
    double SINB;
    double PAR;
    double PARDIF;
    double PARDIR;
	double FGROS;
	double PI = 3.141592653589793238462643383279502884197169399375;


    if(AMAX > 0. && LAI > 0) {
        for (int i = 0; i < 3; i++) {
            HOUR = 12.0 + 0.5 * DAYL * XGAUSS[i];
            SINB = std::max(0., SINLD + COSLD * cos(2 * PI * (HOUR + 12) / 24));
            PAR = 0.5 * AVRAD * SINB * (1.+0.4 * SINB) / DSINBE;
            PARDIF = std::min(PAR, SINB * DifPP);
            PARDIR = PAR-PARDIF;
            //call function
            FGROS = ASSIM (AMAX,EFF,LAI,KDif,SINB,PARDIR,PARDIF);
            DTGA = DTGA + FGROS * WGAUSS[i];

        }
        DTGA = DTGA * DAYL;

    }
    return DTGA;
}
