/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 

Author: C. Rappoldt, January 1986, revised June 1990


Chapter 15 in documentation WOFOST Version 4.1 (1988)

This routine calculates the rate of capillary flow or percolation between groundwater table and root zone. The
stationary flow is found by integration of dZ = K.d(MH)/(K + FLW), where Z= height above groundwater, MH= matric head,
K= conductivity and FLW= chosen flow. In an iteration loop the correct flow is found.
The integration goes at most over four intervals : [0,45],[45,170], [170,330] and [330, MH-rootzone] (last one on logarithmic scale).

*/

#include <algorithm>
#include <cmath>
#include <vector>
#include "wofost.h"
#include "SimUtil.h"


double SUBSOL(double PF, double D, std::vector<double> CONTAB) { // flow is output

//15.1 declarations and constants
      
      int I3, IINT = 0, IMAX;
      double FLOW = 0;
      double DEL[4], PFGAU[12], HULP[12], CONDUC[12];
      double ELOG10 = 2.302585;
      double PGAU[3] = {.1127016654, .5, .8872983346};
      double WGAU[3] = {.2777778, .4444444, .2777778};
      double START[4] = {0., 45., 170., 330.};
      double LOGST4 = 2.518514;
      double PFSTAN[9] = {.705143, 1.352183, 1.601282, 1.771497, 2.031409, 2.192880, 2.274233, 2.397940, 2.494110};

      //*
      //SAVE;

//15.2 calculation of matric head and check on small pF
      double PF1 = PF;
      double D1  = D;
      double MH  = std::exp(ELOG10*PF1);
      if (PF1 <= 0.){
         double K0 = std::exp( ELOG10 * AFGEN(CONTAB, -1.) );
         FLOW = K0 * (MH/D - 1.);

         return FLOW;
      }

//15.3 number and width of integration intervals
      for(int i = 0; i < 4; i++){
         if (i <= 2) {
            DEL[i] = std::min(START[i + 1], MH) - START[i];
         } else if (i == 4) {
            DEL[i] = PF1 - LOGST4;
		 }
         if(DEL[i] <= 0.){
            for(int j = 0; j < IINT; j++){
               for(int k = 0; k < 3; k++){
                  I3 = 3 * (j - 1) + k;
                  PFGAU[I3] = PFSTAN[I3];
                  if (j == IINT){
                  // the three points in the last interval are calculated
                     if(IINT <= 3) PFGAU[I3] = log10(START[IINT] + PGAU[k] * DEL[IINT]);
                     if(IINT == 4) PFGAU[I3] = LOGST4 + PGAU[k] * DEL[IINT];
                     CONDUC[I3] = std::exp( ELOG10 * AFGEN(CONTAB, PFGAU[I3]) );
                     HULP[I3]   = DEL[j] * WGAU[k] * CONDUC[I3];
                     if(I3 > 9) HULP[I3] = HULP[I3] * ELOG10 * std::exp( ELOG10 * PFGAU[I3] );
                  }
                  else{
                     // the three points in the full-width intervals are standard
                     // variables needed in the loop below
                     CONDUC[I3] = std::exp( ELOG10 * AFGEN(CONTAB, PFGAU[I3]) );
                     HULP[I3]   = DEL[j] * WGAU[k] * CONDUC[I3];
                     if(I3 > 9) HULP[I3] = HULP[I3] * ELOG10 * std::exp( ELOG10 * PFGAU[I3] );
                  }
               }
            }
         }
         IINT = IINT+1;
      }

//15.4 preparation of three-point Gaussian integration


//15.5 setting upper and lower limit
      double FU =  1.27;
      double FL = -1. * std::exp( ELOG10 * AFGEN(CONTAB, PF1));
      if (MH <= D1) FU = 0.;
      if (MH >= D1) FL = 0.;
      if (MH == D1){
         FLOW = (FU+FL)/2.;
         return FLOW;
      };

//15.6 Iteration loop
      IMAX = 3 * IINT;
      for(int i = 0; i < 15; i++){
         double FLW = (FU+FL)/2.;
         double DF  = (FU-FL)/2.;
         if ( (DF < 0.01) && ( (DF/std::abs(FLW)) < 0.1)){
            FLOW = (FU+FL)/2.;
            return FLOW;
         }
         double Z = 0.;
         for(int j = 0 ; j < IMAX; j++) {
            Z = Z + HULP[j]/(CONDUC[j]+FLW);
		 }
         if (Z >= D1) FL = FLW;
         if (Z <= D1) FU = FLW;
      }
      return FLOW;
}

