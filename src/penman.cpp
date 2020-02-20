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

This routine calculates the potential evapo(transpi)ration rates from a free water surface (E0), a bare soil surface (ES0), and a crop canopy (ET0) in mm/d. For these calculations the analysis by Penman is followed (Frere and Popov, 1979; Penman, 1948, 1956, and 1963). Subroutines and functions called: ASTRO, LIMIT.


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


#include "SimUtil.h"
#include "wofost.h"
#include <cmath>

void WofostModel::PENMAN() {

//double LAT, double ELEV, double ANGSTA, double ANGSTB, double TMIN, double TMAX, double AVRAD, double VAP, double WIND2, double ATMTR){

//  psychrometric instrument constant (mbar/Celsius-1)
	double PSYCON = 0.67;
//  albedo for water surface, soil surface and canopy
	double REFCFW = 0.05, REFCFS = 0.15, REFCFC = 0.25;
//  latent heat of evaporation of water (J/kg=J/mm)
	double LHVAP = 2.45e6;
//  Stefan Boltzmann constant (J/m2/d/K4)
	double STBC = 4.9e-3;

//10.2 preparatory calculations
//  mean daily temperature and temperature difference (Celsius)
//  coefficient Bu in wind function, dependent on temperature difference
	double TMPA  = ( atm.TMIN + atm.TMAX ) / 2.; // average temp
	double TDif  = atm.TMAX - atm.TMIN;

	double BU    = 0.54 + 0.35 * LIMIT(0, 1, (TDif - 12.) / 4.);

//  barometric pressure (mbar)
//  psychrometric constant (mbar/Celsius)
	double PBAR  = 1013. * exp(-0.034 * loc.elevation / (TMPA+273.));
	double GAMMA = PSYCON * PBAR/1013.;


//  saturated vapour pressure according to equation of Goudriaan
//  (1977) derivative of SVAP with respect to temperature, i.e.
//  slope of the SVAP-temperature curve (mbar/Celsius);
//  measured vapour pressure not to exceed saturated vapour pressure

	double SVAP  = 6.10588 * exp (17.32491*TMPA / (TMPA+238.102));
	double DELTA = 238.102 * 17.32491 * SVAP / pow((TMPA+238.102), 2);
	double VAP   = std::min(atm.VAP, SVAP);

//  the expression n/N (RELSSD) from the Penman formula is estimated
//  from the Angstrom formula: RI=RA(A+B.n/N) -> n/N=(RI/RA-A)/B,
//  where RI/RA is the atmospheric transmission computed by ASTRO

    double RELSSD = LIMIT(0.,1., (atm.ATMTR - std::abs(loc.AngstromA)) / std::abs(loc.AngstromB));

//  Terms in Penman formula, for water, soil and canopy
//  net outgoing long-wave radiation (J/m2/d) acc. to Brunt (1932)
	double RB  = STBC * pow((TMPA+273.), 4) * (0.56-0.079 * sqrt(VAP)) * (0.1+0.9*RELSSD);

//  net absorbed radiation, expressed in mm/d
    double RNW = (atm.AVRAD * (1.-REFCFW)-RB) / LHVAP;
    double RNS = (atm.AVRAD * (1.-REFCFS)-RB) / LHVAP;
    double RNC = (atm.AVRAD * (1.-REFCFC)-RB) / LHVAP;

//  evaporative demand of the atmosphere (mm/d)
    double EA  = 0.26 * std::max(0.,(SVAP-VAP)) * (0.5+BU * atm.WIND);
    double EAC = 0.26 * std::max(0.,(SVAP-VAP)) * (1.0+BU * atm.WIND);
    
//  Penman formula (1948)
    atm.E0  = (DELTA*RNW+GAMMA*EA)/(DELTA+GAMMA);
    atm.ES0 = (DELTA*RNS+GAMMA*EA)/(DELTA+GAMMA);
    atm.ET0 = (DELTA*RNC+GAMMA*EAC)/(DELTA+GAMMA);

//  Ensure reference evaporation >= 0.
    atm.E0  = std::max(0., atm.E0) / 10;
    atm.ES0 = std::max(0., atm.ES0)  / 10;
    atm.ET0 = std::max(0., atm.ET0)  / 10;
       
}



double SatVapourPressure(double temp) {
    return ( 0.6108 * exp((17.27 * temp) / (237.3 + temp)) );
}

double Celsius2Kelvin(double temp) {
	return ( temp + 273.16 );
}

void WofostModel::PENMAN_MONTEITH() {

/* 
    Calculates reference ET0 based on the Penman-Monteith model.

     This routine calculates the potential evapotranspiration rate from
     a reference crop canopy (ET0) in mm/d. For these calculations the
     analysis by FAO is followed as laid down in the FAO publication
     `Guidelines for computing crop water requirements - FAO Irrigation
     and drainage paper 56 <http://www.fao.org/docrep/X0490E/x0490e00.htm#Contents>`_

    Input variables::

        DAY   -  Python datetime.date object                   -
        LAT   -  Latitude of the site                        degrees
        ELEV  - Elevation above sea level                      m
        TMIN  - Minimum temperature                            C
        TMAX  - Maximum temperature                            C
        AVRAD - Daily shortwave radiation                   J m-2 d-1
        VAP   - 24 hour average vapour pressure               hPa
        WIND2 - 24 hour average windspeed at 2 meter          m/s

    Output is:

        ET0   - Penman-Monteith potential transpiration
                rate from a crop canopy                     [mm/d]
    """
*/
    // psychrometric instrument constant (kPa/Celsius)
    double PSYCON = 0.665;
    // albedo and surface resistance [sec/m] for the reference crop canopy
    double REFCFC = 0.23; 
	double CRES = 70.;
    // latent heat of evaporation of water [J/kg == J/mm] and
    double LHVAP = 2.45E6;
    // Stefan Boltzmann constant (J/m2/d/K4, e.g multiplied by 24*60*60)
    double STBC = 4.903E-3;
    // Soil heat flux [J/m2/day] explicitly set to zero
    double G = 0.;

    // mean daily temperature (Celsius)
    double TMPA = (atm.TMIN + atm.TMAX) / 2.;

    // Vapour pressure to kPa
    double VAP = atm.VAP / 10;

    // atmospheric pressure at standard temperature of 293K (kPa)
    double T = 293.0;
    double PATM = 101.3 * pow((T - (0.0065 * loc.elevation))/T, 5.26);

    // psychrometric constant (kPa/Celsius)
    double GAMMA = PSYCON * PATM * 1.0E-3;

    // Derivative of SVAP with respect to mean temperature, i.e.
    // slope of the SVAP-temperature curve (kPa/Celsius);
    double SVAP_TMPA = SatVapourPressure(TMPA);
    double DELTA = (4098. * SVAP_TMPA)/pow((TMPA + 237.3), 2);

    // Daily average saturated vapour pressure [kPa] from min/max temperature
    double SVAP_TMAX = SatVapourPressure(atm.TMAX);
    double SVAP_TMIN = SatVapourPressure(atm.TMIN);
    double SVAP = (SVAP_TMAX + SVAP_TMIN) / 2.;

    // measured vapour pressure not to exceed saturated vapour pressure
    VAP = std::min(VAP, SVAP);

    // Longwave radiation according at Tmax, Tmin (J/m2/d)
    // and preliminary net outgoing long-wave radiation (J/m2/d)
    double STB_TMAX = STBC * pow(Celsius2Kelvin(atm.TMAX), 4);
    double STB_TMIN = STBC * pow(Celsius2Kelvin(atm.TMIN), 4);
    double RNL_TMP = ((STB_TMAX + STB_TMIN) / 2.) * (0.34 - 0.14 * sqrt(VAP));

    // Clear Sky radiation [J/m2/DAY] from Angot TOA radiation
    double CSKYRAD = (0.75 + (2e-05 * loc.elevation)) * atm.ANGOT;

    if (CSKYRAD > 0) {
        // Final net outgoing longwave radiation [J/m2/day]
        double RNL = RNL_TMP * (1.35 * (atm.AVRAD/CSKYRAD) - 0.35);

        // radiative evaporation equivalent for the reference surface
        // [mm/DAY]
        double  RN = ((1-REFCFC) * atm.AVRAD - RNL)/LHVAP;

        // aerodynamic evaporation equivalent [mm/day]
        double  EA = ((900./(TMPA + 273)) * atm.WIND * (SVAP - VAP));

        // Modified psychometric constant (gamma*)[kPa/C]
        double MGAMMA = GAMMA * (1. + (CRES/208. * atm.WIND));

        // Reference ET in mm/day
        atm.ET0 = (DELTA * (RN-G))/(DELTA + MGAMMA) + (GAMMA * EA)/(DELTA + MGAMMA);
        atm.ET0 = std::max(0., atm.ET0);
    } else {
        atm.ET0 = 0.;
	}
	// ignoring the difference between canopy, soil and water, as canopy is much more important.
    atm.E0  = atm.ET0;
    atm.ES0 = atm.ET0;
}	



void WofostModel::ET() {
	ASTRO();
	//if ((loc.AngstromA > 0) & (loc.AngstromB > 0)) {
		PENMAN();
	//} else {
	//	PENMAN_MONTEITH();
	//}
}



/*
std::vector<double> PENMAN (int DOY, double LAT, double ELEV, double ANGSTA, double ANGSTB, double TMIN, double TMAX, double AVRAD, double VAP, double WIND2, double ATMTR){

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
*/