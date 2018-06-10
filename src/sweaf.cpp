/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 

Authors: D.M. Jansen and C.A. van Diepen, October 1986.

Chapter 20 in documentation WOFOST Version 4.1 (1988)

The fraction of easily available soil water between field capacity and wilting point is a function of the
potential evapotranspiration rate (for a closed canopy) in cm/day, ET0, and the crop group number, CGNR (from
1 (=drought-sensitive) to 5 (=drought-resistent)). The function SWEAF describes this relationship given in tabular
form by Doorenbos and  Kassam (1979) and by Van Keulen and Wolf (1986; p.108, table 20).

*/


using namespace std;
#include <vector>
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
