/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 


Authors: I.G.A.M. Noy and C.A. van Diepen, September 1986
revised van Kraalingen, April, van Diepen, October 1991
Added checks to avoid negative evaporation, Allard de Wit, Sep 2011


Chapter 10 in documentation WOFOST Version 4.1 (1988)

This routine calculates the potential evapo(transpi)ration rates from a free water surface (E0), a bare soil surface
(ES0), and a crop canopy (ET0) in mm/d. For these calculations the analysis by Penman is followed (Frere and Popov, 1979;
Penman, 1948, 1956, and 1963). Subroutines and functions called: ASTRO, LIMIT.


FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)
name   type meaning                                      units  class
----   ---- -------                                      -----  -----
DOY    I4  Day number (Jan 1st = 1)                       -      I
LAT     R4  Latitude of the site                        degrees   I
ELEV    R4  Elevation above sea level                      m      I
ANGSTA  R4  Empirical constant in Angstrom formula         -      I
ANGSTB  R4  Empirical constant in Angstrom formula         -      I
TMIN    R4  Minimum temperature                            C      I
TMAX    R4  Maximum temperature                            C      I
AVRAD   R4  Daily shortwave radiation                   J m-2 d-1 I
VAP     R4  24 hour average vapour pressure               hPa     I
WIND2   R4  24 hour average windspeed at 2 meter          m/s     I
E0      R4  Penman potential evaporation from a free
            water surface                                mm/d     O
ES0     R4  Penman potential evaporation from a moist
            bare soil surface                            mm/d     O
ET0     R4  Penman potential transpiration from a crop
            canopy                                       mm/d     O



*/

#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "SimUtil.h"
#include <iostream>
using namespace std;


//int main(){return 0;}

std::vector<double> PENMAN (int DOY, double LAT, double ELEV, double ANGSTA, double ANGSTB, double TMIN,
                        double TMAX, double AVRAD, double VAP, double WIND2, double ATMTR){

//     psychrometric instrument constant (mbar/Celsius-1)
//     albedo for water surface, soil surface and canopy
//     latent heat of evaporation of water (J/kg=J/mm)
//     Stefan Boltzmann constant (J/m2/d/K4)
      double PSYCON = 0.67, REFCFW = 0.05, REFCFS = 0.15, REFCFC = 0.25,
              LHVAP = 2.45e6, STBC = 4.9e-3;

//10.2 preparatory calculations
//     mean daily temperature and temperature difference (Celsius)
//     coefficient Bu in wind function, dependent on temperature
//     difference
      double TMPA  = ( TMIN + TMAX ) / 2.; // average temp
      double TDif  = TMAX - TMIN;
      double BU    = 0.54 + 0.35 * LIMIT( 0., 1., (TDif - 12.) / 4.);

//     barometric pressure (mbar)
//     psychrometric constant (mbar/Celsius)
      double PBAR  = 1013. * exp(-0.034*ELEV/(TMPA+273.));
      double GAMMA = PSYCON*PBAR/1013.;


//     saturated vapour pressure according to equation of Goudriaan
//     (1977) derivative of SVAP with respect to temperature, i.e.
//     slope of the SVAP-temperature curve (mbar/Celsius);
//     measured vapour pressure not to exceed saturated vapour pressure

      double SVAP  = 6.10588 * exp (17.32491*TMPA/(TMPA+238.102));
      double DELTA = 238.102 * 17.32491 * SVAP / pow((TMPA+238.102), 2);
    //test
    //cout << "SVAP, VAP: " << SVAP << " " << VAP << endl;

      VAP   = std::min(VAP,SVAP);

//     the exprep.SSIon n/N (RELSSD) from the Penman formula is estimated
//     from the Angstrom formula: RI=RA(A+B.n/N) -> n/N=(RI/RA-A)/B,
//     where RI/RA is the atmospheric transmip.SSIon obtained by a
//     to ASTRO:

      //std::vector<double> astroResult = ASTRO(DOY, LAT, AVRAD);
	  //double ATMTR = astroResult[4];
    // ATMR is now an argument to the ASTRO function
      double RELSSD = LIMIT(0.,1.,(ATMTR - std::abs(ANGSTA)) / std::abs(ANGSTB));

//     Terms in Penman formula, for water, soil and canopy

//     net outgoing long-wave radiation (J/m2/d) acc. to Brunt (1932)
      double RB  = STBC * pow((TMPA+273.), 4) * (0.56-0.079 * sqrt(VAP)) * (0.1+0.9*RELSSD);

//     net absorbed radiation, expressed in mm/d
      double RNW = (AVRAD*(1.-REFCFW)-RB)/LHVAP;
      double RNS = (AVRAD*(1.-REFCFS)-RB)/LHVAP;
      double RNC = (AVRAD*(1.-REFCFC)-RB)/LHVAP;
    //test
    //cout << "AVR: " << RB << endl;

//     evaporative demand of the atmosphere (mm/d)
      double EA  = 0.26 * std::max(0.,(SVAP-VAP)) * (0.5+BU*WIND2);
      double EAC = 0.26 * std::max(0.,(SVAP-VAP)) * (1.0+BU*WIND2);
    
//     Penman formula (1948)
      double E0  = (DELTA*RNW+GAMMA*EA)/(DELTA+GAMMA);
    //test
    //cout << DELTA << " " << RNW << " " << GAMMA << " " << EA << endl;
    
      double ES0 = (DELTA*RNS+GAMMA*EA)/(DELTA+GAMMA);
      double ET0 = (DELTA*RNC+GAMMA*EAC)/(DELTA+GAMMA);
    //test
    

//     Ensure reference evaporation >= 0.
      E0  = std::max(0., E0);
      ES0 = std::max(0., ES0);
      ET0 = std::max(0., ET0);
    
    //cout << "ET0: " << ET0 << " EAC: " << EAC << " RNC: " << RNC << endl;
    //test
    //cout << "ES0: " << ES0 << endl;

      E0   = E0 /10.;
      ES0  = ES0/10.;
      ET0  = ET0/10.;
    
    

      std::vector<double> result = {E0, ES0, ET0};

      return result;
}
