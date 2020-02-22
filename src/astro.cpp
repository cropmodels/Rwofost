/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 

Author: Daniel van Kraalingen, April 1991
Modification: Include checks for 0 <= daylength <= 24 hour (Allard de Wit, January 2011)

calculate astronomic daylength, diurnal radiation characteristics such as the atmospheric
transmission, diffuse radiation etc.

 (I=input,O=output,C=control,IN=init,T=time)
 name   type meaning                                    units  class
 ----   ---- -------                                    -----  -----
 DOY    I4  Day number (Jan 1st = 1)                      -      I
 LAT     R4  Latitude of the site                       degrees   I
 AVRAD   R4  Daily shortwave radiation                  J m-2 d-1 I
 DAYL    R4  Astronomical daylength (base = 0 degrees)     h      O
 DAYLP   R4  Astronomical daylength (base =-4 degrees)     h      O
 SINLD   R4  Seasonal offset of sine of solar height       -      O
 COSLD   R4  Amplitude of sine of solar height             -      O
 DifPP   R4  Diffuse irradiation perpendicular to direction of
             light                                      J m-2 s-1 O
 ATMTR   R4  Daily atmospheric transmission                -      O
 DSINBE  R4  Daily total of effective solar height         s      O
*/

#include <math.h>
#include <vector>
#include "wofost.h"


void WofostModel::ASTRO() {
	
	double PI = 3.141592653589793238462643383279502884197169399375;
	
    double ANGLE = -4, RAD = 0.0174533;
    //Error check on latitude
    if (control.latitude > 90 || control.latitude < -90) {
        messages.push_back("latitude: " + std::to_string(control.latitude) + " .it should be between -90 and 90");
		fatalError = true;
    }
    //Declination and solar constant for this day
    double DEC = -asin( sin(23.45 * RAD) * cos(2. * PI * (double(DOY) + 10.)/365.));
    double SC = 1370. * (1. + 0.033 * cos(2. * PI * double(DOY)/365.));

    //calculation of daylength from intermediate variables SINLD, COSLD and AOB
    atm.SINLD = sin(RAD * control.latitude) * sin(DEC);
    atm.COSLD = cos(RAD * control.latitude) * cos(DEC);
    double AOB = atm.SINLD / atm.COSLD;

// For very high latitudes and days in summer and winter a limit is inserted
// to avoid math errors when daylength reaches 24 hours in summer or 0 hours in winter.
// Calculate solution for base=0 degrees
   if (AOB > 1.0) {
        atm.DAYL = 24.0;
        atm.DSINB  = 3600. * (atm.DAYL * atm.SINLD);
        atm.DSINBE = 3600. * (atm.DAYL * (atm.SINLD + 0.4 * (pow(atm.SINLD, 2) + pow(atm.COSLD, 2)*0.5)));
    } else if (AOB < -1.0) {
        atm.DAYL = 0.;
        atm.DSINB  = 3600. * (atm.DAYL * atm.SINLD);
        atm.DSINBE = 3600. * (atm.DAYL * (atm.SINLD + 0.4 * (pow(atm.SINLD, 2) + pow(atm.COSLD, 2)*0.5)));
    } else {
        atm.DAYL  = 12.0 * (1. + 2. * asin(AOB) / PI);
        //integrals of sine of solar height
        atm.DSINB = 3600. * (atm.DAYL * atm.SINLD + 24. * atm.COSLD * sqrt(1. - pow(AOB, 2)) / PI);
        atm.DSINBE = 3600. * (atm.DAYL * (atm.SINLD + 0.4 * (pow(atm.SINLD, 2) + pow(atm.COSLD, 2) * 0.5)) + 12. * atm.COSLD * (2. + 3. * 0.4 * atm.SINLD) * sqrt(1. - pow(AOB, 2)) / PI);	
	}

    //Calculate solution for base=-4 (ANGLE) degrees
    double AOB_CORR = (-sin(ANGLE * RAD) + atm.SINLD) / atm.COSLD;
    if (AOB_CORR > 1.0) {
        atm.DAYLP = 24.0;
    } else if (AOB_CORR < -1.0) {
        atm.DAYLP = 0.0;
    } else {
        atm.DAYLP = 12.0 * (1. + 2. * asin(AOB_CORR)/PI);
    }

    //extraterrestrial radiation and atmospheric transmission
    atm.ANGOT  = SC * atm.DSINB;
    //Check for DAYL=0 as in that case the angot radiation is 0 as well
    if (atm.DAYL > 0.0)  {
        atm.ATMTR = atm.AVRAD / atm.ANGOT;
    } else {
        atm.ATMTR = 0;
    }

    //estimate fraction diffuse irradiation
    double FRDif;
    if (atm.ATMTR > 0.75) {
        FRDif = 0.23;
    } else if (atm.ATMTR <= 0.75 && atm.ATMTR > 0.35) {
        FRDif = 1.33 - 1.46 * atm.ATMTR;
    } else if (atm.ATMTR <= 0.35 && atm.ATMTR > 0.07) {
        FRDif = 1. - 2.3 * pow((atm.ATMTR - 0.07), 2);
    } else {
        FRDif = 1.;
    }

    atm.DifPP = FRDif * atm.ATMTR * 0.5 * SC;
}
