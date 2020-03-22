/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2 

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1. 

In routine WATGW the simulation of the soil water balance is performed for soils influenced by the presence of groundwater. Two situations are distinguished: with or without artificial drainage. The soil water balance is calculated for a cropped field in the water-limited production situation. WATGW is called by WOFSIM.

Subroutine WATGW is derived from subroutine APPLE of WOFOST Version 4.1.

Author: C.A. van Diepen, February 1989, revised July 1990

The purpose of the calculations is to estimate the daily value of the mean soil moisture content. The soil moisture content influences soil moisture uptake and crop transpiration.

The water balance is driven by rainfall, popssibly buffered as surface storage, and  evapotranspiration. The processes considered are infiltration, soil water retention, the steady state flow between the rootzone and the groundwater table (upward flow is accounted for as capillary rise, downward flow as percolation), and drainage rate. An irrigation term is included but not used. The resulting groundwater depth, moisture and air contents in the root zone are calculated.

The textural profile of the soil is conceived as homogeneous. Three soil depth zones are distinguished: the rooted zone between soil surface and actual rooting depth, the zone between rooting depth and groundwater, and the zone below groundwater level to a reference depth of XDEF=16000 cm, which is used as a formal system boundary. Soil moisture between groundwater and root zone is assumed to be in equilibrium with groundwater (in fact a contradiction with
capillary rise or percolation). A makeshift approach is applied when the groundwater table rises into the rooted zone. Then two zones are distinguished within the rooted zone, a saturated lower part and an unsaturated upper part.

The extension of the root zone from initial rooting depth to maximum rooting depth (crop dependent) is described in subroutine ROOTD. The dynamic output is written to file by subroutine PRIWGW.
+-----------------------------------------------------------------+
| Date:         24 July 1997                                      |
| Author:       Tamme van der Wal                                 |
| Reason:       Adaptation of soil water balance calculations     |
|               Inclusions of methods of WOFOST4.4                |
| Modification: Addition of 2 new variables in site.dat           |
|               which need to be passed to WATFD and WATGW through|
|               WOFSIM. Variable p.SSI was already passed, variable |
|               p.SMLIM is added to parameter list                  |
+-----------------------------------------------------------------+


VARIABLE TYPE Description                                      Units   I/O
IDRAIN  I*4  indicates presence (1) or absence (0) of drains            I
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
ZTI     R*4  initial depth of groundwater table                 cm      I
DD      R*4  effective depth of drains (drainage base)          cm      I
NOTINF  R*4  if FUNRAI=0 non-infiltrating fraction of rainfall          I
             if FUNRAI=1 maximum non-infiltrating fraction
EVWMX   R*4  maximum evaporation rate from shaded water surface cm d-1  I
EVSMX   R*4  maximum evaporation rate from shaded soil surface  cm d-1  I
TRA     R*4  actual transpiration rate                          cm d-1  I
             Note: TRA is calculated in EVTRA called by CROPSI
SMW     R*4  soil moisture content at wilting point           cm3 cm-3  O
CRAIRC  R*4  critical air content                             cm3 cm-3  O
ZT      R*4  actual depth of groundwater table                   cm     O
SM      R*4  actual soil moisture content                     cm3 cm-3  O
RAIN    R*4  daily rainfall                                     cm d-1  I
SM0     R*4  soil porosity                                    cm3 cm-3  O
SMFCF   R*4  soil moisture content at field capacity          cm3 cm-3  O

*/

#include "wofost.h"
#include "subsol.h"
#include "SimUtil.h"
#include <math.h>


void WofostModel::WATGW_initialize() {
    //!!!  DATA XDEF/16000./
    double XDEF = 1000.;
    //     mathematical parameters
    double PGAU[3] = {0.1127016654, 0.5, 0.8872983346};
    double WGAU[3] = {0.2777778, 0.4444444, 0.2777778};
//     infiltration parameters WOFOST41
//      DATA p.NINFTB/0.0,0.00, 0.5,0.12, 1.0,0.29,
//                   2.0,0.71, 3.0,0.91, 7.0,1.00, 8*0./
//     infiltration parameters WOFOST_WRR
    soil.p.NINFTB = {0.0, 0.0, 0.5, 0.0, 1.5, 1.0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.};
    
    soil.p.SMFCF = AFGEN(soil.p.SMTAB, log10(200.));
    soil.p.SMW = AFGEN(soil.p.SMTAB, log10(16000.));
    soil.p.SM0 = AFGEN(soil.p.SMTAB, -1.);
    
    soil.p.K0 = pow(10., AFGEN(soil.p.CONTAB, -1.));

    int ILSM = soil.p.SMTAB.size();
    soil.p.PFTAB.resize(ILSM);
    for(int i = 2; i <= ILSM; i = i + 2){
        soil.p.PFTAB[ILSM - i] = soil.p.SMTAB[i - 1];
        soil.p.PFTAB[ILSM + 1 - i] = soil.p.SMTAB[i - 2];
    }
    soil.RTDF = 0.;
    //        old rooting depth

    //------------------------------------------------------
//        soil air volume above watertable at equilibrium
//------------------------------------------------------
//        table SDEFTB is calculated, containing the cumulative amount of
//        air as a function of height above groundwater under equilibrium
//        conditions. the table is calculated for the following values of
//        height (= matric head) : 0,2,4,8,16,32,64,128,256,.....16384.
//        method is 3 point gaup.SSIan integration of SM on each interval.
//        Table DEFDTB is the inverse function of SDEFTB.


    crop.s.RDOLD = crop.s.RD;
    soil.MH0 = 0.;
    soil.MH1 = 2.;
    soil.SDEFTB.resize(30);
    soil.DEFDTB.resize(30);
    soil.SDEFTB[0] = 0.;
    soil.SDEFTB[1] = 0.;
    soil.DEFDTB[0] = 0.;
    soil.DEFDTB[1] = 0.;

    int i2 = 0;
    for(int i = 2; i <= 15; i++){
        i2 = 2*i;
        soil.SDEFTB[i2 - 2] = soil.MH1;
        soil.SDEFTB[i2 - 1] = soil.SDEFTB[i2 - 3];
        for(int j = 0; j < 3; j++){
            soil.SDEFTB[i2 - 1] = soil.SDEFTB[i2 - 1] + WGAU[j] * (soil.MH1 - soil.MH0) * (soil.p.SM0 - AFGEN(soil.p.SMTAB, log10(soil.MH0 + (soil.MH1 - soil.MH0)*PGAU[j])));
        }
        soil.DEFDTB[i2 - 2] = soil.SDEFTB[i2 - 1];
        soil.DEFDTB[i2 - 1] = soil.SDEFTB[i2 - 2];
        soil.MH0 = soil.MH1;
        soil.MH1 = 2 * soil.MH1;
    }
        //-----------------------------------------------------
//        initial state variables of the water balance
//-----------------------------------------------------
    soil.ss = soil.p.SSI;
    soil.ZT = LIMIT(0.1, XDEF, soil.p.ZTI);
    if(soil.p.IDRAIN == 1){
        soil.ZT = std::max(soil.ZT, soil.p.DD);
    }
    //        amount of air in soil below rooted zone
    soil.SUBAIR = AFGEN(soil.SDEFTB, soil.ZT - crop.s.RD);

    //        amount of moisture in soil below rooted zone
    soil.WZ = (XDEF - crop.s.RD) * soil.p.SM0 - soil.SUBAIR;
    soil.WZI = soil.WZ;
    //        equilibrium amount of soil moisture in rooted zone

    soil.WE = soil.p.SM0 * crop.s.RD + soil.SUBAIR - AFGEN(soil.SDEFTB, soil.ZT);
    //        equilibrium amount of moisture above drains up to the surface
    soil.WEDTOT = soil.p.SM0 * soil.p.DD - AFGEN(soil.SDEFTB, soil.p.DD);
//        initial moisture content in rooted zone
    if(soil.ZT < crop.s.RD + 100.){
        //           groundwater in or close to rootzone
        soil.W = soil.WE;
    } else{
    // groundwater well below rootzone
        soil.W = soil.p.SMFCF * crop.s.RD;
    }
    soil.SM = soil.W/crop.s.RD;
    soil.WI = soil.W;
    // soil evaporation, days since last rain
    soil.DSLR = 1.;
    if(soil.SM <= AFGEN(soil.p.SMTAB, 3.0)){
        soil.DSLR = 5.;
    }
    //----------------------------------------------------------
    //        all summation variables are initially set at zero
    //----------------------------------------------------------
//    soil.EVST = 0.;
//    soil.EVWT = 0.;
//    soil.TSR = 0.;
//    soil.CRT = 0.;
//    soil.PERCT = 0.;
//    soil.WDRT = 0.;
//    soil.DRAINT = 0.;
    //--------------------------------------------
    //        all rates are initially set at zero
    //--------------------------------------------
    soil.EVS = 0.;
    soil.EVW = 0.;
    atm.RAIN = 0.;
    soil.RIN = 0.;
    soil.RIRR = 0.;
    soil.DW = 0.;
    soil.PERC = 0.;
    soil.CR = 0.;
    soil.DMAX = 0.;
    soil.DZ = 0.;


}

void WofostModel::WATGW_rates() {
    //-----------------------------------------------------------------------
//        rates of the water balance
//-----------------------------------------------------------------------

//        actual transpiration rate
//        N.B.: transpiration rate is calculated in EVTRA called by CROPSI
//        actual evaporation rates ...

    soil.EVW = 0.;
    soil.EVS = 0.;
    //        ... from surface water if surface storage more than 1 cm, ...
    if(soil.ss > 1.){
        soil.EVW = soil.EVWMX;
        //           ... else from soil surface
    } else {
        //! in WOFOST 4.1 :   evs = evsmx * limit(0.,1.,(sm-p.SMW/3.)/(p.SM0-p.SMW/3.))
        if(soil.RIN >= 1.){
            soil.EVS = soil.EVSMX;
            soil.DSLR = 1.;
        } else{
            soil.DSLR = soil.DSLR + 1;
            double EVSMXT = soil.EVSMX * (sqrt(soil.DSLR) - sqrt(soil.DSLR - 1.));
            soil.EVS = std::min(soil.EVSMX, EVSMXT + soil.RIN);
        }
    }
    //        preliminary infiltration rate
    double RINPRE;
    if(soil.ss > 0.1){
        //           with surface storage, infiltration limited by p.SOPE
        //!!         Next line replaced TvdW 24-jul-97
        //!!            AVAIL  = SS+(RAIN+RIRR-EVW)`
        double AVAIL = soil.ss + (atm.RAIN * (1. - soil.p.NOTINF) + soil.RIRR - soil.EVW);
        RINPRE = std::min(soil.p.SOPE, AVAIL);
    } else {
        if(soil.p.IFUNRN == 0) { 
			RINPRE = (1. - soil.p.NOTINF) * atm.RAIN + soil.RIRR + soil.ss; 
		} else { // if(soil.p.IFUNRN == 1){
            RINPRE = (1. - soil.p.NOTINF * AFGEN(soil.p.NINFTB, atm.RAIN)) * atm.RAIN + soil.RIRR + soil.ss;
        }
    }
    //        indicator for groundwater table within (-) or below (+) rootzone
    double ZTMRD = soil.ZT - crop.s.RD;
    //        capillary flow through the lower root zone boundary
    //        no capillary flow if groundwater table is within rooted zone
    soil.CR = 0.;
    soil.PERC = 0.;

    if(ZTMRD > 0.){
        //           groundwater table below rooted zone:
        //           equilibrium amount of soil moisture in rooted zone
        soil.WE = soil.p.SM0 * crop.s.RD + soil.SUBAIR - AFGEN(soil.SDEFTB, soil.ZT);
        //           soil suction
        soil.PF = AFGEN(soil.p.PFTAB, soil.SM);
        //           calculate capillary flow
        //call subsol;
        double FLOW = SUBSOL(soil.PF, ZTMRD, soil.p.CONTAB);
        //           flow is accounted for as capillary rise or percolation
        if(FLOW >= 0.){
            soil.CR = std::min(FLOW, std::max(soil.WE - soil.W, 0.));
        }
        if(FLOW <= 0.){
            soil.PERC = -1. * std::max(FLOW, std::min(soil.WE - soil.W, 0.));
        }
        // hypothesis : for rice percolation is limited to p.K0/20
        if (crop.p.IAIRDU){
            soil.PERC = std::min(soil.PERC, 0.05 * soil.p.K0);
        }
    }
    //        drainage rate
    if(soil.p.IDRAIN == 1 && soil.ZT < soil.p.DD){
        //           capacity of artificial drainage system
        double DR1 = 0.2 * soil.p.K0;
        double DR2;
        if(ZTMRD <= 0.){
            //              ground water above drains and within rootzone
            DR2 = std::max(0., soil.W + std::max(0., soil.p.DD - crop.s.RD) * soil.p.SM0 - soil.WEDTOT);
            soil.DMAX = std::min(DR1, DR2);
        } else{
            //              groundwater above drains and below root zone ; available
            //              is the difference between equilibrium water above drains
            //              and equilibrium water above groundwater level (both until
            //              root zone).

            DR2 = (AFGEN(soil.SDEFTB, soil.p.DD - crop.s.RD) - soil.SUBAIR);
            soil.DMAX = std::min(DR1, DR2);
        }

    } else{
        //           no drainage if drains are absent or if groundwater is below drains
        soil.DMAX = 0.;
    }
    //        change in the groundwater depth
    //        and adjustment of infiltration rate

    if(ZTMRD <= 0.) {
        double AIRC;
        // groundwater table within rootzone
        // air concentration above groundwater in cm3/cm3
        if (soil.ZT >= 0.1) {
            AIRC = (crop.s.RD * soil.p.SM0 - soil.W) / soil.ZT;
        } else {
			AIRC = 0; // RH ???
		}
        //           infiltration rate not to exceed available soil air volume
        soil.PERC = soil.DMAX;
        soil.RIN = std::min(RINPRE, AIRC * soil.ZT + crop.TRA + soil.EVS + soil.PERC);
        soil.DZ = (crop.TRA + soil.EVS + soil.PERC - soil.RIN) / AIRC;
        //           check if groundwater table stays within rooted zone
        if (soil.DZ > crop.s.RD - soil.ZT) {
            //              groundwater table will drop below rooted zone;
            //              in order to maintain a stable moisture content in the rooted
            //              zone during this transition, water is recovered from the subsoil.
            //              In the water balance of the rooted zone this amount of water is
            //              accounted for as CR (capillary rise).
            soil.CR = (soil.DZ - (crop.s.RD - soil.ZT)) * AIRC;
            //              new equilibrium groundwater depth, based on the soil water
            //              deficit
            soil.DZ = (AFGEN(soil.DEFDTB, soil.CR) + crop.s.RD - soil.ZT);
        }
    }else{
        //           groundwater table below rootzone
        double DEF1 = soil.SUBAIR + (soil.DMAX + soil.CR + soil.PERC);
        //           groundwater not to exceed RD in current time step
        if(DEF1 < 0.){
            soil.PERC = soil.PERC + DEF1;
        }
        soil.DZ = (AFGEN(soil.DEFDTB, DEF1) + crop.s.RD - soil.ZT);
        //           infiltration rate not to exceed available soil air volume
        soil.RIN = std::min(RINPRE, (soil.p.SM0 - soil.SM - 0.0004) * crop.s.RD + crop.TRA + soil.EVS + soil.PERC - soil.CR);
    }
        //        rate of change in amount of moisture in the root zone
    soil.DW = crop.TRA - soil.EVS - soil.PERC + soil.CR + soil.RIN;
        //---------------
        //        output
        //---------------
        //        output to WOFOST.OUT of daily soil water variables
//        RAINT1 = RAINT;
//        EVW1   = EVW;
//        EVS1   = EVS;
//        SM1    = SM;
//        SS1    = SS;
//        ZT1    = ZT;
}


void WofostModel::WATGW_states(){
//----------------------------------------------------------------------
//     dynamic calculations
//     integrals of the water balance:  summation and state variables
//----------------------------------------------------------------------

//        transpiration
    // total evaporation from surface water layer and/or soil
    //soil.EVWT += soil.EVW;
    //soil.EVST += soil.EVS;
    // surface storage and runoff
    double SSPRE = soil.ss + (atm.RAIN + soil.RIRR - soil.EVW - soil.RIN);
    soil.ss = std::min(SSPRE, soil.p.SSMAX);
    //soil.TSR += (SSPRE - soil.ss);
    // amount of water in rooted zone
    soil.W += soil.DW;

    //        total capillary rise or percolation
    // soil.CRT = soil.CRT + soil.CR;
    //soil.PERCT = soil.PERCT + soil.PERC;
    //        total drainage
    // soil.DRAINT = soil.DRAINT + soil.DMAX;
    //        groundwater depth
    soil.ZT += soil.DZ;
    //        amount of air and water below rooted zone
    soil.SUBAIR = AFGEN(soil.SDEFTB, soil.ZT - crop.s.RDOLD);
    double XDEF = 1000.;
    soil.WZ = (XDEF - crop.s.RDOLD) * soil.p.SM0 - soil.SUBAIR;

    //---------------------------------------------
    //        change of rootzone subsystem boundary
    //---------------------------------------------
    //        calculation of amount of soil moisture in new rootzone
    if(crop.s.RD - crop.s.RDOLD > 0.001){
        //           save old value SUBAIR, new values SUBAIR and WZ
        double SUBAI0 = soil.SUBAIR;
        soil.SUBAIR = AFGEN(soil.SDEFTB, soil.ZT - crop.s.RD);
        soil.WZ = (XDEF - crop.s.RD) * soil.p.SM0 - soil.SUBAIR;
        //           water added to rooted zone by root growth
        double WDR = soil.p.SM0 * (crop.s.RD - crop.s.RDOLD) - (SUBAI0 - soil.SUBAIR);
        //           total water addition to rootzone by root growth
        //soil.WDRT = soil.WDRT + WDR;
        soil.W += WDR;
    }
    // mean soil moisture content in rooted zone
    soil.SM = soil.W / crop.s.RD;
    // calculating mean soil moisture content over growing period
    //soil.SUMSM += soil.SM;
    // save rooting depth
    crop.s.RDOLD = crop.s.RD;
    //------------------------------
    //        check on waterlogging
    //------------------------------
    //        finish conditions due to lasting lack of oxygen in root zone
    //        (non-rice crops only)
    if((!crop.p.IAIRDU) && soil.RTDF >= 10.){
        std::string m ("Crop failure due to waterlogging");
        messages.push_back(m);
        fatalError = true;
    } else{
        if(soil.ZT < 10.){
            soil.RTDF++;
        } else { 
            soil.RTDF = 0.;
        }
    }

}










