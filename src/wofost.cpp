/*
Authors: Robert Hijmans and Huang Fang
Date: June 2016

License: GNU General Public License (GNU GPL) v. 2
*/

#include <vector>
#include "wofost.h"
#include "SimUtil.h"
#include <math.h>
#include <string.h>
//#include <iostream>


bool WofostModel::weather_step() {
	if (time >= wth.tmin.size()) {
		fatalError = true;
		messages.push_back("reached end of weather data");
		return false;
	} else {
		atm.TMIN = wth.tmin[time];
		atm.TMAX = wth.tmax[time];
		atm.TEMP  = (atm.TMIN + atm.TMAX) / 2.;
		atm.DTEMP = (atm.TMAX + atm.TEMP) / 2.;

		atm.AVRAD = wth.srad[time] * 1000;
		atm.WIND = wth.wind[time];
		atm.VAP = wth.vapr[time] * 10;
		atm.RAIN = wth.prec[time] / 10 ; // cm !

/*
		//seven day running average of minimum temperature
		int start = time-7;
		start = std::max(start, 0);
		double n = time - start;
		crop.TMINRA = accumulate(wth.tmin.begin() + start, wth.tmin.begin() + time, 0.0) / n;
*/

		DOY = doy_from_days(wth.date[time]);

		ASTRO();
		PENMAN(); // E0, ES0, ET0
		PENMAN_MONTEITH(); // ET0

	//(evapo)transpiration rates
		EVTRA();

		//soil.EVWMX = atm.E0;
		//soil.EVSMX = atm.ES0;

	}
	return true;
}


void WofostModel::model_output(){
	if (control.output_option == "TEST") {
		output.values.insert(output.values.end(),
			{double(step), atm.ANGOT, atm.ATMTR, atm.COSLD, atm.DAYL,
				atm.DAYLP, atm.DifPP, atm.DSINBE, atm.SINLD, soil.EVWMX,
				crop.s.TSUM, crop.r.DVR, crop.s.DVS, soil.EVS, crop.s.LAI, crop.LASUM,
				crop.s.SAI, crop.PGASS, crop.s.RD, soil.SM,  crop.Fl, crop.Fo, crop.Fr, crop.Fs,
				crop.PMRES,	crop.s.TAGP, crop.TRA, crop.TRAMX, crop.RFTRA,
				crop.s.WRT, crop.s.WLV, crop.s.WST, crop.s.WSO,
				crop.s.TWRT, crop.s.TWLV, crop.s.TWST, crop.s.TWSO, crop.s.GRLV, crop.SLAT
			}
		);
	} else {
		output.values.insert(output.values.end(),
			{double(step), crop.s.TSUM, crop.s.DVS, crop.s.LAI,
				crop.s.WRT, crop.s.WLV, crop.s.WST, crop.s.WSO,
				crop.TRA, soil.EVS, soil.EVW, soil.SM
			}
		);
	}
}



void WofostModel::model_initialize() {

	fatalError = false;
	if (wth.date.size() < 1) {
		std::string m = "no weather data";
	    messages.push_back(m);
	    fatalError = true;
	}

// start time (relative to weather data)
	if (control.modelstart < wth.date[0]) {
		std::string m = "model cannot start before beginning of the weather data";
	    messages.push_back(m);
	    fatalError = true;
		//return;
	} else if (control.modelstart > wth.date[wth.date.size()-1]) {
		std::string m = "model cannot start after the end of the weather data";
	    messages.push_back(m);
	    fatalError = true;
		//return;
	} else {
		time=0;
		while (wth.date[time] < control.modelstart) {
			time++;
		}
	}

	if (control.ISTCHO == 0) { // model starts at emergence)
		ISTATE = 3;
	} else if (control.ISTCHO == 1) { // model starts at sowing
		ISTATE = 1;
	} else {
		std::string m = "start_sowing (ISTCHO) must be 0 or 1";
	    messages.push_back(m);
	    fatalError = true;
	}
	//	if (control.ISTCHO == 2) { // model starts prior to earliest possible sowing date
	//	ISTATE = 0;
	//	STDAY_initialize();
	//}

//	ISTATE = 3;

	if (control.output_option == "TEST") {
		output.names = {"step", "ANGOT", "ATMTR", "COSLD", "DAYL", "DAYLP", "DIFPP",
			"DSINBE", "SINLD", "EVWMX", "TSUM", "DVR", "DVS", "EVS", "LAI", "LASUM", "SAI", "PGASS", "RD", "SM", "FL", "FO", "FR", "FS", "PMRES", "TAGP",
			"TRA", "TRAMX", "RFTRA", "WRT", "WLV", "WST", "WSO",
			"TWRT", "TWLV", "TWST", "TWSO", "GRLV", "SLAT"};
	} else {
		output.names = {"step", "TSUM", "DVS", "LAI", "WRT", "WLV", "WST", "WSO", "TRA", "EVS", "EVW", "SM"};
	}

	output.values.resize(0);
	output.values.reserve(output.names.size() * 150);

	DOY = doy_from_days(wth.date[time]);
    crop.alive = true;

	soil_initialize();
	/*
	if(control.nutrient_limited){
		npk_soil_dynamics_initialize();
		npk_translocation_initialize();
		npk_demand_uptake_initialize();
	}
	*/

	if (ISTATE == 1) {
		crop.s.DVS = -0.1;
	} else {
		crop.s.DVS = 0;
    }

	crop.s.WRT = 0;
    crop.s.TADW = 0;
    crop.s.WST = 0;
    crop.s.WSO = 0;
    crop.s.WLV = 0;
    crop.LV[0] = 0;
    crop.LASUM = 0;
    crop.s.LAIEXP = 0;
    crop.s.LAI = 0;
	crop.KDif = 0;
    crop.s.SAI = 0;
    crop.s.PAI = 0;
    crop.s.RD = crop.p.RDI;
	crop.s.TSUM = 0;
	crop.s.TSUME = 0;
	crop.r.DTSUME = 0;
    crop.TRA = 0;
    crop.RFTRA = 0;
	crop.r.GASS = 0;
	crop.s.GRLV = 0;

	// adjusting for CO2 effects
    double CO2AMAXadj = AFGEN(crop.p.CO2AMAXTB, control.CO2);
	for(size_t i=1; i<crop.p.AMAXTB.size(); i=i+2) {
		crop.p.AMAXTB[i] = crop.p.AMAXTB[i] * CO2AMAXadj;
	}
    double CO2EFFadj = AFGEN(crop.p.CO2EFFTB, control.CO2);
	for(size_t i=1; i<crop.p.CO2EFFTB.size(); i=i+2) {
		crop.p.CO2EFFTB[i] = crop.p.CO2EFFTB[i] * CO2EFFadj;
	}
	double CO2TRAadj = AFGEN(crop.p.CO2TRATB, control.CO2);
	for(size_t i=1; i<crop.p.CO2TRATB.size(); i=i+2) {
		crop.p.CO2TRATB[i] = crop.p.CO2TRATB[i] * CO2TRAadj;
	}

}

void WofostModel::force_states() {
	if (control.useForce) {
		if (forcer.force_DVS)  crop.s.DVS = forcer.DVS[time];
		if (forcer.force_LAI)  crop.s.LAI = forcer.LAI[time];
		if (forcer.force_SAI)  crop.s.SAI = forcer.SAI[time];
		if (forcer.force_PAI)  crop.s.PAI = forcer.PAI[time];
		if (forcer.force_SM )  soil.SM  = forcer.SM[time];
		if (forcer.force_WLV)  crop.s.WLV = forcer.WLV[time];
		if (forcer.force_WRT)  crop.s.WRT = forcer.WRT[time];
		if (forcer.force_WSO)  crop.s.WSO = forcer.WSO[time];
		if (forcer.force_WST)  crop.s.WST = forcer.WST[time];
		if (forcer.force_RFTRA) crop.RFTRA = forcer.RFTRA[time];
		if (forcer.force_FR)   crop.Fr = forcer.FR[time];
		if (forcer.force_FL)   crop.Fl = forcer.FL[time];
	}
}


void WofostModel::model_run() {

	step = 1;
	//npk_step = 0;
	unsigned cropstart_step = step + control.cropstart;

	model_initialize();

// model can start long before crop and run the soil water balance
	bool crop_emerged = false;

	force_states();

	if (ISTATE == 1) {
		crop.s.DVS = -0.1;
	}
//

	while (! crop_emerged) {
		force_states();

		weather_step();
		//if(control.nutrient_limited){
		//	npk_soil_dynamics_rates();
		//} else{
		soil_rates();
		//}
		//soil.EVWMX = atm.E0;
		//soil.EVSMX = atm.ES0;

		if (step >= cropstart_step) {
			if (ISTATE == 0 ) { // find day of sowing
				STDAY();
			} else if (ISTATE == 1) { // find day of emergence
				//ugly
				crop.s.DVS = crop.s.DVS + crop.r.DVR;
				if (control.useForce & forcer.force_DVS) {
					crop.s.DVS = forcer.DVS[time];
				}

				crop.s.TSUME = crop.s.TSUME + crop.r.DTSUME;
				if (crop.s.DVS >= 0) {
					ISTATE = 3;
					crop_emerged = true;
					crop.s.DVS = 0;
				} else {
					crop.r.DTSUME = LIMIT(0., crop.p.TEFFMX - crop.p.TBASEM, atm.TEMP - crop.p.TBASEM);
					crop.r.DVR = 0.1 * crop.r.DTSUME / crop.p.TSUMEM;
				}
			} else {
				crop_emerged = true;
			}
		}
		if (fatalError) {
			break;
		}

		if (!crop_emerged) {
			model_output();
			//if(control.nutrient_limited){
			//	npk_soil_dynamics_states();
			//} else {
				soil_states();
			//}
			time++;
			step++;
		}
	}
	crop.emergence = step;
	crop_initialize();

	unsigned maxdur = step + control.IDURMX;
	/*
	unsigned maxdur;
	if (control.IENCHO == 1) {
		maxdur = cropstart_step + control.IDAYEN;
	} else if (control.IENCHO == 2) {
		maxdur = step + control.IDURMX;
	} else if (control.IENCHO == 3) {
		maxdur = std::min(cropstart_step + control.IDAYEN, step + control.IDURMX);
	} else {
		// throw error
		maxdur = step + 365;
	}
	*/


//	crop_initialize();

	while ((crop.alive) && (step < maxdur)) {

		force_states();

		if (! weather_step()) break;
		crop_rates();
		//if (control.nutrient_limited){
		//	npk_soil_dynamics_rates();
		//} else {
			soil_rates();
		//}
		model_output();
		crop_states();
		//if(control.nutrient_limited){
		//	npk_soil_dynamics_states();
		//} else {
			soil_states();
		//}

		time++;
		step++;
		if (fatalError) {
			break;
		}
	}
	//if (control.IENCHO == 1) {
	if (!control.stop_maturity) {
		// should continue until maxdur if water balance if IENCHO is 1
	    crop.TRA = 0;
		while (step < maxdur) {
			weather_step();
			soil_rates();
			// assuming that the crop has been harvested..
			// not checked with fortran
			soil.EVWMX = atm.E0;
			soil.EVSMX = atm.ES0;
			model_output();
			//crop_states();
			soil_states();
			time++;
			step++;
		}
	}
}


