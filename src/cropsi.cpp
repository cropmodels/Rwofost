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
1) New parameter is read in: DVSI
             |2) KDif, EFF and SSA are now functions of DVS or DTEMP, 
calculated using KDIFTB, EFFTB and SSATB and the AFGEN function.
SSA renamed to SSI following PCSE

RH, 18 March 2020
DVSI set to a fixed value of 0 (or -0.2 if crop is seeded)
DVSI set to a fixed value of 2 


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
    double RMRES = (crop.pn.p.RMR * crop.s.WRT + crop.pn.p.RML * crop.s.WLV + crop.pn.p.RMS * crop.s.WST + crop.pn.p.RMO * crop.s.WSO);
    RMRES *= AFGEN(crop.pn.p.RFSETB, crop.s.DVS);
    double TEFF = pow(crop.pn.p.Q10, ((atm.TEMP - 25.)/10.));
    crop.vn.PMRES = RMRES * TEFF;
}
*/


void WofostModel::crop_initialize() {
// 2.6    initial crop conditions at emergence or transplanting
	crop.IDANTH = -99;
//  DOANTH = false;
	crop.s.DVS = crop.p.DVSI;
	
	crop.s.TSUM = 0;
	crop.Fr = AFGEN(crop.p.FRTB, crop.s.DVS);
	crop.Fl = AFGEN(crop.p.FLTB, crop.s.DVS);
	crop.Fs = AFGEN(crop.p.FSTB, crop.s.DVS);
	crop.Fo = AFGEN(crop.p.FOTB, crop.s.DVS);

	//crop.SLA.resize(control.IDURMX + control.cropstart);
	//crop.LV.resize(control.IDURMX + control.cropstart);
	//crop.LVAGE.resize(control.IDURMX + control.cropstart);
	crop.SLA[0] = AFGEN(crop.p.SLATB, crop.s.DVS);
	crop.LVAGE[0] = 0.;
	crop.ILVOLD = 1;

	//2.6.2  initial state variables of the crop
	crop.s.DWRT = 0;
	crop.s.DWLV = 0;
	crop.s.DWST = 0;
	crop.s.DWSO = 0;

    //emergence
    crop.s.WRT = crop.Fr * crop.p.TDWI;
    crop.s.TADW = (1. - crop.Fr) * crop.p.TDWI;
    crop.s.WST = crop.Fs * crop.s.TADW;
    crop.s.WSO  = crop.Fo * crop.s.TADW;
    crop.s.WLV  = crop.Fl * crop.s.TADW;

	crop.s.TWRT = crop.s.WRT;
	crop.s.TWLV = crop.s.WLV;

	crop.s.TWST = crop.s.WST;
	crop.s.TWSO = crop.s.WSO;
	
    crop.p.LAIEM = crop.s.WLV * crop.SLA[0];
    crop.LV[0] = crop.s.WLV;
    crop.LASUM = crop.p.LAIEM;
    crop.s.LAIEXP = crop.p.LAIEM;
    crop.s.SAI = crop.s.WST * AFGEN(crop.p.SSATB, crop.s.DVS);
    crop.s.PAI = crop.s.WSO * crop.p.SPA;
    crop.s.LAI = crop.LASUM + crop.s.SAI + crop.s.PAI;

	crop.TMINRA = 0.;
	for(int i = 0; i < 7; i++){
		crop.TMNSAV[i] = -99;
	}
	
	crop.TMNSAV.reserve(7);
	
	//if (control.nutrient_limited){
	//	npk_crop_dynamics_initialize();
	//}

	//vernalization_initialize();

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
        crop.TMINRA += crop.TMNSAV[i];
        j++;
      }
    }
    crop.TMINRA = crop.TMINRA/double(j);


    crop.r.DTSUM = AFGEN(crop.p.DTSMTB, atm.TEMP);
	//vernalization_rates();
    if(crop.s.DVS < 1.){
      //effects of daylength and temperature on development during vegetative phase
		crop.r.DVR = crop.r.DTSUM / crop.p.TSUM1;
		double DVRED;
		if (crop.p.IDSL >= 1) {
			DVRED = LIMIT(0.,1.,(atm.DAYLP- crop.p.DLC)/(crop.p.DLO - crop.p.DLC));
		} else {
			DVRED = 1;
		}
		//crop.r.DVR = crop.r.DVR * DVRED * crop.r.VERNFAC;
		crop.r.DVR = crop.r.DVR * DVRED;

    } else { //development during generative phase
		crop.r.DVR = crop.r.DTSUM / crop.p.TSUM2;
    }

  // 2.20   daily dry matter production
  //gross assimilation and correction for sub-optimum average day temperature

	crop.AMAX = AFGEN(crop.p.AMAXTB, crop.s.DVS) * AFGEN(crop.p.TMPFTB, atm.DTEMP);
	crop.KDif = AFGEN(crop.p.KDIFTB, crop.s.DVS);
	crop.EFF = AFGEN(crop.p.EFFTB, atm.DTEMP);

	double DTGA = TOTASS();

  //correction for low minimum temperature potential assimilation in kg CH2O per ha
	DTGA = DTGA * AFGEN(crop.p.TMNFTB, crop.TMINRA);
	crop.PGASS = DTGA * 30./44.;

  //water stress reduction
	double reduction = crop.TRA / crop.TRAMX;

  //  crop.r.GASS = crop.PGASS * crop.TRA / crop.TRAMX;
  // nutrient status and reduction factor
	//if (control.nutrient_limited) {
	//	npk_stress();
	//	reduction = std::min(crop.vn.NPKREF, crop.TRA / crop.TRAMX);
	//}  else {
	//reduction = crop.TRA / crop.TRAMX;
	//}
	
    // water stress reduction
    // Select minimum of nutrient and water stress
  //double reduction = min(crop.vn.NPKREF, crop.TRA / crop.TRAMX);

	crop.r.GASS = crop.PGASS * reduction;
  //crop.r.GASS = crop.PGASS * crop.TRA / crop.TRAMX;

  //respiration and partitioning of carbohydrates between growth and maintenance respiration
	double RMRES = (crop.p.RMR * crop.s.WRT + crop.p.RML * crop.s.WLV + crop.p.RMS * crop.s.WST + crop.p.RMO * crop.s.WSO);
	RMRES *= AFGEN (crop.p.RFSETB, crop.s.DVS);
	double TEFF  = pow(crop.p.Q10, ((atm.TEMP - 25.)/10.));
	crop.PMRES = RMRES * TEFF;
	double MRES  = std::min(crop.r.GASS, crop.PMRES);
	double ASRC  = crop.r.GASS - MRES;

  //DM partitioning factors, and dry matter increase
	crop.Fr = AFGEN(crop.p.FRTB, crop.s.DVS);
	crop.Fl = AFGEN(crop.p.FLTB, crop.s.DVS);
	crop.Fs = AFGEN(crop.p.FSTB, crop.s.DVS);
	crop.Fo = AFGEN(crop.p.FOTB, crop.s.DVS);

	crop.TRANRF = crop.TRA/crop.TRAMX;   //commented previously

/*
	if (control.nutrient_limited){

		if (crop.TRANRF < crop.vn.NNI) {
	//          Water stress is more severe than nitrogen stress and the
	//          partitioning follows the original LINTUL2 assumptions
	//          Note: we use specifically nitrogen stress not nutrient stress!!!

	// RH: this seems wrong. FR is affected but this is not corrected for
	// sum of FR, FL, FS, FO shouldd remain 1.
			double FRTMOD = std::max( 1., 1 / (crop.TRANRF + 0.5));
			crop.Fr = std::min(0.6, crop.Fr * FRTMOD);
		} else {
	// Nitrogen stress is more severe than water stress resulting in
	// less partitioning to leaves and more to stems
			double FLVMOD = exp(- crop.pn.NPART * (1.0 - crop.vn.NNI));
			double FL = (crop.Fl * FLVMOD);
			crop.Fs = crop.Fs + crop.Fl - FL;
			crop.Fl = FL;
		}
	}
*/

	double CVF = 1./((crop.Fl/crop.p.CVL + crop.Fs/crop.p.CVS + crop.Fo/crop.p.CVO)*(1. - crop.Fr) + crop.Fr/crop.p.CVR);

	double DMI = CVF * ASRC;

  //check on partitioning
	double FCHECK = crop.Fr + (crop.Fl + crop.Fs + crop.Fo) * (1 - crop.Fr) - 1;
    //test
	if (fabs(FCHECK) > 0.0001){
		std::string m = "Error in partitioning functions on step " + std::to_string(step) + " FCHECK = " + std::to_string(FCHECK) + " FR = " + std::to_string(crop.Fr) 
		+ " FL = " + std::to_string(crop.Fl) + " FS = " + std::to_string(crop.Fs)
		+ " FO = " + std::to_string(crop.Fo);
		messages.push_back(m);
		//fatalError = true;
	}
  //check on carbon balance
	double CCHECK = (crop.r.GASS - MRES - (crop.Fr + (crop.Fl + crop.Fs + crop.Fo)*(1. - crop.Fr)) * DMI/CVF)/ std::max(0.0001, crop.r.GASS);
  
	if (fabs(CCHECK) > 0.0001){
		std::string m = "Carbon balance leak (CCHECK in cropsi) on step " + std::to_string(step);
		messages.push_back(m);
		//fatalError = true;
	}

  //growth rate by plant organ
  //growth rate roots and aerial parts
	double ADMI = (1. - crop.Fr) * DMI;
	if (control.useForce & forcer.force_ADMI) {
		ADMI = forcer.ADMI[time];
	} 	

	double GRRT = crop.Fr * DMI;
	if (control.useForce & forcer.force_DMI) {
		GRRT = crop.Fr * forcer.DMI[time];
	} 	
	
	crop.r.DRRT = crop.s.WRT * AFGEN(crop.p.RDRRTB, crop.s.DVS);
	crop.r.GWRT = GRRT - crop.r.DRRT;

  //growth rate leaves
  //weight of new leaves
	if (control.useForce & forcer.force_FL) {
		crop.s.GRLV = forcer.FL[time] * ADMI;
	} else {
		crop.s.GRLV = crop.Fl * ADMI;
	}
  //death of leaves due to water stress or high LAI
	double DSLV1 = crop.s.WLV * (1. - crop.TRA/crop.TRAMX) * crop.p.PERDL;
	double LAICR = 3.2 / crop.KDif;
	double DSLV2 = crop.s.WLV * LIMIT(0., 0.03, 0.03 * (crop.s.LAI - LAICR)/LAICR);
	crop.DSLV = std::max(DSLV1, DSLV2);

  //determine extra death due to exceeding of life p.SPAN of leaves leaf death is imposed on array until no more leaves have to die or all leaves are gone
	double REST = crop.DSLV;
	int i1 = crop.ILVOLD;
	while (REST > crop.LV[i1 - 1] && i1 >= 1){
		REST = REST - crop.LV[i1 - 1];
		i1--;
	}

  //check if some of the remaining leaves are older than p.SPAN, sum their weights
	double DALV = 0.;
	if (crop.LVAGE[i1 - 1] > crop.p.SPAN && REST > 0. && i1 >= 1) {
		DALV = crop.LV[i1 - 1] - REST;
		REST = 0.;
		i1--;
	}
	while (i1 >= 1 && crop.LVAGE[i1 - 1] > crop.p.SPAN) {
		DALV += crop.LV[i1 - 1];
		i1--;
	}
  //death rate leaves and growth rate living leaves
	crop.r.DRLV = crop.DSLV + DALV;

  //physiologic ageing of leaves per time step
	crop.r.FYSDEL = std::max(0., (atm.TEMP - crop.p.TBASE)/(35. - crop.p.TBASE));
	crop.SLAT = AFGEN(crop.p.SLATB, crop.s.DVS);

  //leaf area not to exceed exponential growth curve
	if (crop.s.LAIEXP < 6) {
		double DTEFF = std::max(0., atm.TEMP - crop.p.TBASE);
		crop.r.GLAIEX = crop.s.LAIEXP * crop.p.RGRLAI * DTEFF;
    //source-limited increase in leaf area
		double GLASOL = crop.s.GRLV * crop.SLAT;
    //sink-limited increase in leaf area
		double GLA = std::min(crop.r.GLAIEX, GLASOL);
    //adjustment of specific leaf area of youngest leaf class
		if (crop.s.GRLV > 0.) {
			crop.SLAT = GLA / crop.s.GRLV;
		}
	}
	
	//growth rate stems
	double GRST = crop.Fs * ADMI;
	crop.r.DRST = AFGEN(crop.p.RDRSTB, crop.s.DVS) * crop.s.WST;
	crop.r.GWST = GRST - crop.r.DRST;

  //growth rate storage organs
	crop.r.GWSO = crop.Fo * ADMI;
	crop.r.DRSO = 0.;

	//if(control.nutrient_limited){
	//	npk_crop_dynamics_rates();
	//}

	ROOTD_rates();

}


void WofostModel::crop_states() {

//  if(crop.ITOLD == 1){
//    crop.ITOLD = 3;
    //return?
//  }

  //phenological development stage and temperature sum
	crop.s.DVS += crop.r.DVR;
	crop.s.TSUM += crop.r.DTSUM;
  //save date of anthesis, adjust development stage
	if( crop.s.DVS >= 1. && crop.IDANTH < 0 ){
		crop.IDANTH = int( step ) - crop.emergence;
// RH: DVS is above 1 for the first time and set back
//       to exactly 1. That seems to be a strange fudge.
 		crop.s.DVS = 1.;
	}
	//vernalization_states();


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
		crop.LVAGE[j] = crop.LVAGE[j - 1] + crop.r.FYSDEL;
	}
	crop.ILVOLD++;

  //new leaves in class 1
	crop.LV[0] = crop.s.GRLV;
	crop.SLA[0] = crop.SLAT;
	crop.LVAGE[0] = 0.;
  //calculation of new leaf area and weight
	crop.LASUM = 0.;
	crop.s.WLV = 0.;
	for (int j = 0; j < crop.ILVOLD; j++){
		crop.LASUM += crop.LV[j] * crop.SLA[j];
		crop.s.WLV += crop.LV[j];
	}

	crop.s.LAIEXP += crop.r.GLAIEX;
  //dry weight of living plant organs and total above ground biomass
	crop.s.WRT += crop.r.GWRT;
	crop.s.WST += crop.r.GWST;
	crop.s.WSO += crop.r.GWSO;
	crop.s.TADW = crop.s.WLV + crop.s.WST + crop.s.WSO;
	
  //dry weight of dead plant organs
	crop.s.DWRT += crop.r.DRRT;
	crop.s.DWLV += crop.r.DRLV;
	crop.s.DWST += crop.r.DRST;
	crop.s.DWSO += crop.r.DRSO;

  //dry weight of dead and living plant organs
	crop.s.TWRT = crop.s.WRT + crop.s.DWRT;
	crop.s.TWLV = crop.s.WLV + crop.s.DWLV;
	crop.s.TWST = crop.s.WST + crop.s.DWST;
	crop.s.TWSO = crop.s.WSO + crop.s.DWSO;
	crop.s.TAGP = crop.s.TWLV + crop.s.TWST + crop.s.TWSO;

    // from pcse: SSA * WST = Stem Area Index (SAI)
	// crop.SSA = AFGEN(crop.p.SSATB, crop.s.DVS);
	crop.s.SAI = crop.s.WST * AFGEN(crop.p.SSATB, crop.s.DVS);

	// pod area index
	crop.s.PAI = crop.s.WSO * crop.p.SPA;

	//leaf area index
	crop.s.LAI = crop.LASUM + crop.s.SAI + crop.s.PAI;

	//if(control.nutrient_limited){
	//	npk_crop_dynamics_states();
	//}

	ROOTD_states();

	if(crop.s.DVS >= crop.p.DVSEND){
		crop.alive = false;
	} 	else if(crop.s.LAI <= 0.002 && crop.s.DVS > 0.5) {
		//messages.push_back("no living leaves (anymore)");
		crop.alive = false;
	}
}

