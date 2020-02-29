/*
C++ code by Huang Fang and Robert Hijmans
Date: July 2016

License: GNU General Public License (GNU GPL) v. 2

Based on FORTRAN code WOFOST version 7.1.7, release September 2013
Copyright 1988, 2013 Alterra, Wageningen-UR, Licensed under the EUPL, Version 1.1.

Author: C.A. van Diepen, February 1989, revised July 1990, December 1992

In this routine the simulation of the potential or the water-limited crop growth is performed.

Modifications since WOFOST Version 4.1:
- 3 extra parameters for the description of exponential growth of young leaves : RGRLAI LAIEM DVSEND
- simulation of crop emergence

Tamme van der Wal (25-July-1997)
1) New parameter is read in: p.DVSI                 |
2) KDIFTB, EFFTB and SSATB are now read in from data files and KDif, EFF and SSA are calculated using the AFGEN function.
KDif, EFF and SSA which are now functions of DVS or DTEMP

SSA renamed to SSI following PCSE

FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)
name   type meaning                                    units  class
----   ---- -------                                    -----  -----
IDEM    I4  Day of emergence                                     I/O
//DOANTH  R4  Day of anthesis                                      O
IDHALT  I4  Day that simulation is halted                        I/O
LAT     R4  Latitude of the site                         degr    I
AVRAD   R4  Daily short wave radiation                   J/m2/d  I
TMIN    R4  minimum daily air temperature                _C      I
TMAX    R4  maximum daily air temperature                _C      I
E0      R4  potential evaporation rate from water surf   cm d-1  I
ES0     R4  potential evaporation rate from soil surf    cm d-1  I
ET0     R4  potential evapotranspiration rate            cm d-1  I
CRFILE  C   name of input file with crop data                    I
IUPL    I4  unit of input file with crop data                    I
IUOUT   I4  unit of output file with simulation results          I
IULOG   I4  unit of log file                                     I
SM      R4  soil moisture content in the rooted zone    cm3 cm-3 I
p.SM0     R4  soil porosity, saturated moisture content   cm3 cm-3 I
p.SMFCF   R4  soil moisture content at field capacity     cm3 cm-3 I
p.SMW     R4  soil moisture content at wilting point      cm3 cm-3 I
p.CRAIRC  R4  critical air content in the rootzone        cm3 cm-3 I
EVWMX   R4  maximum evaporation rate from water surface cm d-1   I
EVSMX   R4  maximum evaporation rate from soil surface  cm d-1   I or O
TRA     R4  crop transpiration rate                     cm d-1   I or O
???FR   R4  fraction of dry matter increase
            partitioned to the roots                      -      O
RRI     R4  root length growth rate                     cm d-1   O
IAIRDU  I4  indicates presence (1) or absence (0) of
            airducts in the roots.                               O
            (1) means tolerating water-saturated soils
RDI     R4  initial rooting depth                         cm     O
p.RDMCR   R4  crop-dependent maximum rooting depth          cm     O
*/

#include <math.h>
#include "wofost.h"

/*
// used for npk
void WofostModel::maintanance_respiration() {
    double RMRES = (crop.pn.p.RMR * crop.WRT + crop.pn.p.RML * crop.WLV + crop.pn.p.RMS * crop.WST + crop.pn.p.RMO * crop.WSO);
    RMRES *= AFGEN(crop.pn.p.RFSETB, crop.DVS);
    double TEFF = pow(crop.pn.p.Q10, ((atm.TEMP - 25.)/10.));
    crop.vn.PMRES = RMRES * TEFF;
}
*/


void WofostModel::crop_initialize() {
// 2.6    initial crop conditions at emergence or transplanting
	crop.IDANTH = -99;
//  DOANTH = false;
	crop.DVS = crop.p.DVSI;
	crop.TSUM = 0.;
	crop.FR = AFGEN(crop.p.FRTB, crop.DVS);
	crop.FL = AFGEN(crop.p.FLTB, crop.DVS);
	crop.FS = AFGEN(crop.p.FSTB, crop.DVS);
	crop.FO = AFGEN(crop.p.FOTB, crop.DVS);
	crop.SLA[0] = AFGEN(crop.p.SLATB, crop.DVS);

	crop.LVAGE[0] = 0.;
	crop.ILVOLD = 1;

	//2.6.2  initial state variables of the crop
	crop.DWRT = 0;
	crop.DWLV = 0;
	crop.DWST = 0;
	crop.DWSO = 0;

    //emergence
    crop.WRT = crop.FR * crop.p.TDWI;
    crop.TADW = (1. - crop.FR) * crop.p.TDWI;
    crop.WST = crop.FS * crop.TADW;
    crop.WSO  = crop.FO * crop.TADW;
    crop.WLV  = crop.FL * crop.TADW;

	crop.TWRT = crop.WRT;
	crop.TWLV = crop.WLV;

	crop.TWST = crop.WST;
	crop.TWSO = crop.WSO;
	
    crop.p.LAIEM = crop.WLV * crop.SLA[0];
    crop.LV[0] = crop.WLV;
    crop.LASUM = crop.p.LAIEM;
    crop.LAIEXP = crop.p.LAIEM;
    crop.SAI = crop.WST * AFGEN(crop.p.SSATB, crop.DVS);
    crop.PAI = crop.WSO * crop.p.SPA;
    crop.LAI = crop.LASUM + crop.SAI + crop.PAI;

	crop.TMINRA = 0.;
	for(int i = 0; i < 7; i++){
		crop.TMNSAV[i] = -99;
	}
	
	crop.TMNSAV.reserve(7);
	if (control.nutrient_limited){
		npk_crop_dynamics_initialize();
	}

}




void WofostModel::crop_rates() {

/*
	if (crop.TMNSAV.size() < 7) {
		crop.TMNSAV.push_back(atm.TMIN);
	} else {
		std::move(crop.TMNSAV.begin()+1, crop.TMNSAV.end(), crop.TMNSAV.begin());
		crop.TMNSAV[6] = atm.TMIN;
	}
	crop.TMINRA = accumulate(crop.TMNSAV.begin(), crop.TMNSAV.end(), 0.0) / crop.TMNSAV.size(); 
*/

	std::move(crop.TMNSAV.begin()+1, crop.TMNSAV.end(), crop.TMNSAV.begin());
    crop.TMNSAV[6] = atm.TMIN;
    crop.TMINRA = 0.;
    int j = 0;
    for (int i = 0; i < 7; i++) {
      if (crop.TMNSAV[i] != -99) {
        crop.TMINRA = crop.TMINRA + crop.TMNSAV[i];
        j++;
      }
    }
    crop.TMINRA = crop.TMINRA/double(j);


    crop.DTSUM = AFGEN(crop.p.DTSMTB, atm.TEMP);
    if(crop.DVS < 1.){
      //effects of daylength and temperature on development during vegetative phase
		crop.DVR = crop.DTSUM / crop.p.TSUM1;
		if (crop.p.IDSL >= 1) {
			double DVRED = LIMIT(0.,1.,(atm.DAYLP- crop.p.DLC)/(crop.p.DLO - crop.p.DLC));
			crop.DVR = crop.DVR * DVRED;
		}

    } else { //development during generative phase
		crop.DVR = crop.DTSUM / crop.p.TSUM2;
    }

  // 2.20   daily dry matter production
  //gross assimilation and correction for sub-optimum average day temperature

	crop.AMAX = AFGEN(crop.p.AMAXTB, crop.DVS) * AFGEN(crop.p.TMPFTB, atm.DTEMP);
	crop.KDif = AFGEN(crop.p.KDIFTB, crop.DVS);
	crop.EFF = AFGEN(crop.p.EFFTB, atm.DTEMP);

	double DTGA = TOTASS();

  //correction for low minimum temperature potential assimilation in kg CH2O per ha
	DTGA = DTGA * AFGEN(crop.p.TMNFTB, crop.TMINRA);
	crop.PGASS = DTGA * 30./44.;

  //(evapo)transpiration rates
	EVTRA();
  //water stress reduction

  //  crop.GASS = crop.PGASS * crop.TRA / crop.TRAMX;
  // nutrient status and reduction factor
	double reduction;
	if (control.nutrient_limited) {
		npk_stress();
		reduction = std::min(crop.vn.NPKREF, crop.TRA / crop.TRAMX);
	}  else {
		reduction = crop.TRA / crop.TRAMX;
	}
    // water stress reduction
    // Select minimum of nutrient and water stress
  //double reduction = min(crop.vn.NPKREF, crop.TRA / crop.TRAMX);

	crop.GASS = crop.PGASS * reduction;
  //crop.GASS = crop.PGASS * crop.TRA / crop.TRAMX;

  //respiration and partitioning of carbohydrates between growth and maintenance respiration
	double RMRES = (crop.p.RMR * crop.WRT + crop.p.RML * crop.WLV + crop.p.RMS * crop.WST + crop.p.RMO * crop.WSO);
	RMRES *= AFGEN (crop.p.RFSETB, crop.DVS);
	double TEFF  = pow(crop.p.Q10, ((atm.TEMP - 25.)/10.));
	crop.PMRES = RMRES * TEFF;
	double MRES  = std::min(crop.GASS, crop.PMRES);
	double ASRC  = crop.GASS - MRES;

  //DM partitioning factors, and dry matter increase
	crop.FR = AFGEN(crop.p.FRTB, crop.DVS);
	crop.FL = AFGEN(crop.p.FLTB, crop.DVS);
	crop.FS = AFGEN(crop.p.FSTB, crop.DVS);
	crop.FO = AFGEN(crop.p.FOTB, crop.DVS);

	crop.TRANRF = crop.TRA/crop.TRAMX;   //commented previously

	if (control.nutrient_limited){

		if (crop.TRANRF < crop.vn.NNI) {
	//          Water stress is more severe than nitrogen stress and the
	//          partitioning follows the original LINTUL2 assumptions
	//          Note: we use specifically nitrogen stress not nutrient stress!!!

	// RH: this seems wrong. FR is affected but this is not corrected for
	// sum of FR, FL, FS, FO shouldd remain 1.
			double FRTMOD = std::max( 1., 1 / (crop.TRANRF + 0.5));
			crop.FR = std::min(0.6, crop.FR * FRTMOD);
		} else {
	// Nitrogen stress is more severe than water stress resulting in
	// less partitioning to leaves and more to stems
			double FLVMOD = exp(- crop.pn.NPART * (1.0 - crop.vn.NNI));
			double FL = (crop.FL * FLVMOD);
			crop.FS = crop.FS + crop.FL - FL;
			crop.FL = FL;
		}
	}

	double CVF = 1./((crop.FL/crop.p.CVL + crop.FS/crop.p.CVS + crop.FO/crop.p.CVO)*(1. - crop.FR) + crop.FR/crop.p.CVR);

	double DMI = CVF * ASRC;

  //check on partitioning
	double FCHECK = crop.FR + (crop.FL + crop.FS + crop.FO) * (1 - crop.FR) - 1;
    //test
	if (fabs(FCHECK) > 0.0001){
		std::string m = "Error in partitioning functions on step " + std::to_string(step) + " FCHECK = " + std::to_string(FCHECK) + " FR = " + std::to_string(crop.FR) 
		+ " FL = " + std::to_string(crop.FL) + " FS = " + std::to_string(crop.FS)
		+ " FO = " + std::to_string(crop.FO);
		messages.push_back(m);
		//fatalError = true;
	}
  //check on carbon balance
	double CCHECK = (crop.GASS - MRES - (crop.FR + (crop.FL + crop.FS + crop.FO)*(1. - crop.FR)) * DMI/CVF)/ std::max(0.0001, crop.GASS);
  
	if (fabs(CCHECK) > 0.0001){
		std::string m = "Carbon balance leak (CCHECK in cropsi) on step " + std::to_string(step);
		messages.push_back(m);
		//fatalError = true;
	}

  //growth rate by plant organ
  //growth rate roots and aerial parts
	double ADMI = (1. - crop.FR) * DMI;
	if (control.useForce & forcer.force_ADMI) {
		ADMI = forcer.ADMI[time];
	} 	

	double GRRT = crop.FR * DMI;
	if (control.useForce & forcer.force_DMI) {
		GRRT = crop.FR * forcer.DMI[time];
	} 	
	
	crop.DRRT = crop.WRT * AFGEN(crop.p.RDRRTB, crop.DVS);
	crop.GWRT = GRRT - crop.DRRT;

  //growth rate leaves
  //weight of new leaves
	if (control.useForce & forcer.force_FL) {
		crop.GRLV = forcer.FL[time] * ADMI;
	} else {
		crop.GRLV = crop.FL * ADMI;
	}
  //death of leaves due to water stress or high LAI
	double DSLV1 = crop.WLV * (1. - crop.TRA/crop.TRAMX) * crop.p.PERDL;
	double LAICR = 3.2 / crop.KDif;
	double DSLV2 = crop.WLV * LIMIT(0., 0.03, 0.03 * (crop.LAI - LAICR)/LAICR);
	crop.DSLV = std::max(DSLV1, DSLV2);

  //determine extra death due to exceeding of life p.SPAN of leaves leaf death is imposed on array until no more leaves have to die or all leaves are gone
	double REST = crop.DSLV;
	int i1 = crop.ILVOLD;
	while(REST > crop.LV[i1 - 1] && i1 >= 1){
		REST = REST - crop.LV[i1 - 1];
		i1--;
	}

  //check if some of the remaining leaves are older than p.SPAN, sum their weights
	double DALV = 0.;
	if (crop.LVAGE[i1 - 1] > crop.p.SPAN && REST > 0. && i1 >= 1){
		DALV = crop.LV[i1 - 1] - REST;
		REST = 0.;
		i1--;
	}
	while(i1 >= 1 && crop.LVAGE[i1 - 1] > crop.p.SPAN){
		DALV = DALV + crop.LV[i1 - 1];
		i1--;
	}
	DALV = DALV;
  //death rate leaves and growth rate living leaves
	crop.DRLV = crop.DSLV + DALV;

  //physiologic ageing of leaves per time step
	crop.FYSDEL = std::max(0., (atm.TEMP - crop.p.TBASE)/(35. - crop.p.TBASE));
	crop.SLAT = AFGEN(crop.p.SLATB, crop.DVS);

  //leaf area not to exceed exponential growth curve
	if (crop.LAIEXP < 6) {
		double DTEFF = std::max(0., atm.TEMP - crop.p.TBASE);
		crop.GLAIEX = crop.LAIEXP * crop.p.RGRLAI * DTEFF;
    //source-limited increase in leaf area
		double GLASOL = crop.GRLV * crop.SLAT;
    //sink-limited increase in leaf area
		double GLA = std::min(crop.GLAIEX, GLASOL);
    //adjustment of specific leaf area of youngest leaf class
		if (crop.GRLV > 0.) {
			crop.SLAT = GLA / crop.GRLV;
		}
	}
	
	//growth rate stems
	double GRST = crop.FS * ADMI;
	crop.DRST = AFGEN(crop.p.RDRSTB, crop.DVS) * crop.WST;
	crop.GWST = GRST - crop.DRST;

  //growth rate storage organs
	crop.GWSO = crop.FO * ADMI;
	crop.DRSO = 0.;

	if(control.nutrient_limited){
		npk_crop_dynamics_rates();
	}

	ROOTD_rates();

}


void WofostModel::crop_states() {

//  if(crop.ITOLD == 1){
//    crop.ITOLD = 3;
    //return?
//  }

  //phenological development stage and temperature sum
	crop.DVS = crop.DVS + crop.DVR;
	crop.TSUM = crop.TSUM + crop.DTSUM;
  //save date of anthesis, adjust development stage
	if( crop.DVS >= 1. && crop.IDANTH < 0 ){
		crop.IDANTH = int( step ) - crop.emergence;

/* RH: DVS is above 1 for the first time and set back
     to exactly 1. That seems to be a strange fudge.
     figure out why this is, or remove?	*/
		crop.DVS = 1.;
//    DOANTH = true;
	}


  //leaf death is imposed on array until no more leaves have to die or all leaves are gone
	double DSLVT = crop.DSLV;
	int i1 = crop.ILVOLD;
	while(DSLVT > 0. && i1 >= 1){
		if (DSLVT >= crop.LV[i1 - 1]){
			DSLVT = DSLVT - crop.LV[i1 - 1];
			crop.LV[i1 - 1] = 0.;
			i1--;
		} else {
			crop.LV[i1 - 1] = crop.LV[i1 - 1] - DSLVT;
			DSLVT = 0.;
		}
	}

	while(crop.LVAGE[i1 - 1] >= crop.p.SPAN && i1 >= 1){
		crop.LV[i1 - 1] = 0.;
		i1--;
	}

	crop.ILVOLD = i1;
  //shifting of contents, integration of physiological age

	for(int j = crop.ILVOLD; j >= 1; j--){
		crop.LV[j] = crop.LV[j - 1];
		crop.SLA[j] = crop.SLA[j - 1];
		crop.LVAGE[j] = crop.LVAGE[j - 1] + crop.FYSDEL;
	}
	crop.ILVOLD++;

  //new leaves in class 1
	crop.LV[0] = crop.GRLV;
	crop.SLA[0] = crop.SLAT;
	crop.LVAGE[0] = 0.;
  //calculation of new leaf area and weight
	crop.LASUM = 0.;
	crop.WLV = 0.;
	for (int j = 0; j < crop.ILVOLD; j++){
		crop.LASUM += crop.LV[j] * crop.SLA[j];
		crop.WLV += crop.LV[j];
	}

	crop.LAIEXP += crop.GLAIEX;
  //dry weight of living plant organs and total above ground biomass
	crop.WRT += crop.GWRT;
	crop.WST += crop.GWST;
	crop.WSO += crop.GWSO;
	crop.TADW = crop.WLV + crop.WST + crop.WSO;
	
  //dry weight of dead plant organs
	crop.DWRT += crop.DRRT;
	crop.DWLV += crop.DRLV;
	crop.DWST += crop.DRST;
	crop.DWSO += crop.DRSO;

  //dry weight of dead and living plant organs
	crop.TWRT = crop.WRT + crop.DWRT;
	crop.TWLV = crop.WLV + crop.DWLV;
	crop.TWST = crop.WST + crop.DWST;
	crop.TWSO = crop.WSO + crop.DWSO;
	crop.TAGP = crop.TWLV + crop.TWST + crop.TWSO;

    // from pcse: SSA * WST = Stem Area Index (SAI)
	// crop.SSA = AFGEN(crop.p.SSATB, crop.DVS);
	crop.SAI = crop.WST * AFGEN(crop.p.SSATB, crop.DVS);

	// pod area index
	crop.PAI = crop.WSO * crop.p.SPA;

	//leaf area index
	crop.LAI = crop.LASUM + crop.SAI + crop.PAI;

	if(control.nutrient_limited){
		npk_crop_dynamics_states();
	}

	ROOTD_states();

	if(crop.DVS >= crop.p.DVSEND){
		crop.alive = false;
	} 	else if(crop.LAI <= 0.002 && crop.DVS > 0.5) {
		//messages.push_back("no living leaves (anymore)");
		crop.alive = false;
	}
}

