/*
Authors: Robert Hijmans and Huang Fang
Date: June 2016

License: GNU General Public License (GNU GPL) v. 2
*/


#include <Rcpp.h>
using namespace Rcpp;
#include <vector>
#include "SimUtil.h"
#include "wofost.h"
#include "R_interface_util.h"
//#include <iostream>

// [[Rcpp::export]]
NumericMatrix wofost(List crop, DataFrame weather, List soil, List control) {


// control parameters
	struct WofostControl cntr;
	struct WofostCrop crp;

	std::vector<long> startvec = longFromList(control, "modelstart");
	cntr.modelstart = startvec[0];

	cntr.cropstart = intFromList(control, "cropstart");
	//cntr.long_output = boolFromList(control, "long_output");

	cntr.IPRODL = intFromList(control, "IPRODL"); // translate IPRDL to IWB
	cntr.IOXWL = intFromList(control, "IOXWL");

	cntr.ISTCHO = intFromList(control, "ISTCHO");
	cntr.IDESOW = intFromList(control, "IDESOW");
	cntr.IDLSOW = intFromList(control, "IDLSOW");

	cntr.IENCHO = intFromList(control, "IENCHO");
	cntr.IDAYEN = intFromList(control, "IDAYEN");
	cntr.IDURMX = intFromList(control, "IDURMX");
	//npk
	cntr.npk_model = OptionalintFromList(control, "npk_model");
	if(cntr.npk_model){
		cntr.NPKdates = longFromList(control, "NPKdates");
		cntr.N_amount = vecFromList(control, "N");
		cntr.P_amount = vecFromList(control, "P");
		cntr.K_amount = vecFromList(control, "K");

		crp.pn.TCNT = doubleFromList(crop, "TCNT");
		crp.pn.TCPT = doubleFromList(crop, "TCPT");
		crp.pn.TCKT = doubleFromList(crop, "TCKT");
		crp.pn.DVSNPK_STOP = doubleFromList(crop, "DVSNPK_STOP");
		crp.pn.NFIX_FR = doubleFromList(crop, "NFIX_FR");
		crp.pn.NPART = doubleFromList(crop, "NPART");
		crp.pn.NMAXSO = doubleFromList(crop, "NMAXSO");
		crp.pn.PMAXSO = doubleFromList(crop, "PMAXSO");
		crp.pn.KMAXSO = doubleFromList(crop, "KMAXSO");
		crp.pn.NMAXRT_FR = doubleFromList(crop, "NMAXRT_FR");
		crp.pn.PMAXRT_FR = doubleFromList(crop, "PMAXRT_FR");
		crp.pn.KMAXRT_FR = doubleFromList(crop, "KMAXRT_FR");
		crp.pn.NMAXST_FR = doubleFromList(crop, "NMAXST_FR");
		crp.pn.PMAXST_FR = doubleFromList(crop, "PMAXST_FR");
		crp.pn.KMAXST_FR = doubleFromList(crop, "KMAXST_FR");
		crp.pn.NRESIDLV = doubleFromList(crop, "NRESIDLV");
		crp.pn.NRESIDST = doubleFromList(crop, "NRESIDST");
		crp.pn.NRESIDRT = doubleFromList(crop, "NRESIDRT");
		crp.pn.PRESIDLV = doubleFromList(crop, "PRESIDLV");
		crp.pn.PRESIDST = doubleFromList(crop, "PRESIDST");
		crp.pn.PRESIDRT = doubleFromList(crop, "PRESIDRT");
		crp.pn.KRESIDLV = doubleFromList(crop, "KRESIDLV");
		crp.pn.KRESIDST = doubleFromList(crop, "KRESIDST");
		crp.pn.KRESIDRT = doubleFromList(crop, "KRESIDRT");
		crp.pn.NCRIT_FR = doubleFromList(crop, "NCRIT_FR");
		crp.pn.PCRIT_FR = doubleFromList(crop, "PCRIT_FR");
		crp.pn.KCRIT_FR = doubleFromList(crop, "KCRIT_FR");
		crp.pn.NLUE_NPK = doubleFromList(crop, "NLUE_NPK");
		crp.pn.NPK_TRANSLRT_FR = doubleFromList(crop, "NPK_TRANSLRT_FR");
		crp.pn.NMAXLV_TB = TBFromList2(crop, "NMAXLV_TB");
		crp.pn.PMAXLV_TB = TBFromList2(crop, "PMAXLV_TB");
		crp.pn.KMAXLV_TB = TBFromList2(crop, "KMAXLV_TB");
	}

	crp.p.TBASEM = doubleFromList(crop, "TBASEM");
	crp.p.TEFFMX = doubleFromList(crop, "TEFFMX");
	crp.p.TSUMEM = doubleFromList(crop, "TSUMEM");
	crp.p.IDSL = intFromList(crop, "IDSL");
	crp.p.DLO = doubleFromList(crop, "DLO");
	crp.p.DLC = doubleFromList(crop, "DLC");
	crp.p.TSUM1 = doubleFromList(crop, "TSUM1");
	crp.p.TSUM2 = doubleFromList(crop, "TSUM2");
	crp.p.DTSMTB = TBFromList2(crop, "DTSMTB");
	crp.p.DVSI = doubleFromList(crop, "DVSI");
	crp.p.DVSEND = doubleFromList(crop, "DVSEND");
	crp.p.TDWI = doubleFromList(crop, "TDWI");
	crp.p.LAIEM = doubleFromList(crop, "LAIEM");
	crp.p.RGRLAI = doubleFromList(crop, "RGRLAI");
	crp.p.SLATB = TBFromList2(crop, "SLATB");
	crp.p.SPA = doubleFromList(crop, "SPA");
	crp.p.SSATB = TBFromList2(crop, "SSATB");
	crp.p.SPAN = doubleFromList(crop, "SPAN");
	crp.p.TBASE = doubleFromList(crop, "TBASE");
	crp.p.CVL = doubleFromList(crop, "CVL");
	crp.p.CVO = doubleFromList(crop, "CVO");
	crp.p.CVR = doubleFromList(crop, "CVR");
	crp.p.CVS = doubleFromList(crop, "CVS");
	crp.p.Q10 = doubleFromList(crop, "Q10");
	crp.p.RML = doubleFromList(crop, "RML");
	crp.p.RMO = doubleFromList(crop, "RMO");
	crp.p.RMR = doubleFromList(crop, "RMR");
	crp.p.RMS = doubleFromList(crop, "RMS");
	crp.p.RFSETB = TBFromList2(crop, "RFSETB");
	crp.p.FRTB = TBFromList2(crop, "FRTB");
	crp.p.FLTB = TBFromList2(crop, "FLTB");
	crp.p.FSTB = TBFromList2(crop, "FSTB");
	crp.p.FOTB = TBFromList2(crop, "FOTB");
	crp.p.PERDL = doubleFromList(crop, "PERDL");
	crp.p.RDRRTB = TBFromList2(crop, "RDRRTB");
	crp.p.RDRSTB = TBFromList2(crop, "RDRSTB");
	crp.p.CFET = doubleFromList(crop, "CFET");
	crp.p.DEPNR = doubleFromList(crop, "DEPNR");
	crp.p.RDI = doubleFromList(crop, "RDI");
	crp.p.RRI = doubleFromList(crop, "RRI");
	crp.p.RDMCR = doubleFromList(crop, "RDMCR");

	crp.p.IAIRDU = intFromList(crop, "IAIRDU");

	crp.p.KDIFTB = TBFromList2(crop, "KDIFTB");
	crp.p.EFFTB = TBFromList2(crop, "EFFTB");
	crp.p.AMAXTB = TBFromList2(crop, "AMAXTB");
	crp.p.TMPFTB = TBFromList2(crop, "TMPFTB");
	crp.p.TMNFTB = TBFromList2(crop, "TMNFTB");

	crp.p.CO2AMAXTB = TBFromList2(crop, "CO2AMAXTB");
	crp.p.CO2EFFTB = TBFromList2(crop, "CO2EFFTB");
	crp.p.CO2TRATB = TBFromList2(crop, "CO2TRATB");


// soil parameters
	struct WofostSoil sol;

	if (cntr.IOXWL != 0) {
		sol.p.SMTAB = TBFromList2(soil, "SMTAB");
	} else { // should be no need to read it; need to check if true
		sol.p.SMTAB = TBFromList2(soil, "SMTAB");
	}
	sol.p.SMW = doubleFromList(soil, "SMW");
	sol.p.SMFCF = doubleFromList(soil, "SMFCF");
	sol.p.SM0 = doubleFromList(soil, "SM0");
	sol.p.CRAIRC = doubleFromList(soil, "CRAIRC");
	sol.p.CONTAB = TBFromList2(soil, "CONTAB");
	sol.p.K0 = doubleFromList(soil, "K0");
	sol.p.SOPE = doubleFromList(soil, "SOPE");
	sol.p.KSUB = doubleFromList(soil, "KSUB");
	sol.p.SPADS = doubleFromList(soil, "SPADS");
	sol.p.SPASS = doubleFromList(soil, "SPASS");
	sol.p.SPODS = doubleFromList(soil, "SPODS");
	sol.p.SPOSS = doubleFromList(soil, "SPOSS");
	sol.p.DEFLIM = doubleFromList(soil, "DEFLIM");


	//soil variables that used to be in the control object
	sol.p.IZT = intFromList(soil, "IZT");  // groundwater present
	sol.p.IFUNRN = intFromList(soil, "IFUNRN");
	sol.p.WAV = doubleFromList(soil, "WAV");
	sol.p.ZTI = doubleFromList(soil, "ZTI");
	sol.p.DD = doubleFromList(soil, "DD");
	sol.p.RDMSOL = doubleFromList(soil, "RDMSOL");

	sol.p.IDRAIN = intFromList(soil, "IDRAIN"); // presence of drains
	sol.p.NOTINF = intFromList(soil, "NOTINF"); // fraction not inflitrating rainfall
	sol.p.SSMAX = doubleFromList(soil, "SSMAX"); // max surface storage
	sol.p.SMLIM = doubleFromList(soil, "SMLIM");
	sol.p.SSI = doubleFromList(soil, "SSI");

	if(cntr.npk_model){
		sol.pn.N_recovery = vecFromList(control, "Nrecovery");
		sol.pn.P_recovery = vecFromList(control, "Precovery");
		sol.pn.K_recovery = vecFromList(control, "Krecovery");
		sol.pn.BG_N_SUPPLY = doubleFromList(soil, "BG_N_SUPPLY");
		sol.pn.BG_P_SUPPLY = doubleFromList(soil, "BG_P_SUPPLY");
		sol.pn.BG_K_SUPPLY = doubleFromList(soil, "BG_K_SUPPLY");
		sol.pn.NSOILBASE = doubleFromList(soil, "NSOILBASE");
		sol.pn.PSOILBASE = doubleFromList(soil, "PSOILBASE");
		sol.pn.KSOILBASE = doubleFromList(soil, "KSOILBASE");
		sol.pn.NSOILBASE_FR = doubleFromList(soil, "NSOILBASE_FR");
		sol.pn.PSOILBASE_FR = doubleFromList(soil, "PSOILBASE_FR");
		sol.pn.KSOILBASE_FR = doubleFromList(soil, "KSOILBASE_FR");

	}


// weather
	DailyWeather wth;
	wth.tmin = doubleFromDF(weather, "tmin");
	wth.tmax = doubleFromDF(weather, "tmax");
	wth.srad = doubleFromDF(weather, "srad");
	wth.prec = doubleFromDF(weather, "prec");
	wth.vapr = doubleFromDF(weather, "vapr");
	wth.wind = doubleFromDF(weather, "wind");
	wth.date = longFromDF(weather, "date");
//	DateVector wdate = dateFromDF(weather, "date");
//	wth.date.resize(wdate.size());
//	for (int i = 0; i < wdate.size(); i++) {
//		wth.date[i] = date(wdate[i].getYear(), wdate[i].getMonth(),  wdate[i].getDay());
//	}

	//stop( "complete reading data" );

//		Rcout << "start " << start[s].getYear() << " " << start[s].getYearday() << endl;
//		Rcout << "weather " << wdate[0].getYear() << " " << wdate[0].getYearday() << endl;

/*		if (start[s] < wdate[0]) {
			stop("start requested before the beginning of the weather data");
		} else if (start[s] > wdate[nwth-1]) {
			stop("start requested after the end of the weather data");
		}
*/
		// absolute to relative time
//		Rcout << "offset: " << int(start[s] - wdate[0]) << endl;

	//cntr.modelstart = start - wth.date[0];
	

	WofostModel m;
	m.crop = crp;
	m.soil = sol;
	m.control = cntr;
	m.wth = wth;

/*
	m.latitude = doubleFromList(control, "latitude");
	m.elevation = doubleFromList(control, "elevation");
	m.ANGSTA = doubleFromList(control, "ANGSTA");
	m.ANGSTB = doubleFromList(control, "ANGSTB");
	m.CO2 = doubleFromList(control, "CO2");
*/
	m.model_run();

// handle messages
	// std::vector<std::string>  messages;
	// bool fatalError;

	if (m.fatalError) {
		for (size_t i = 0; i < m.messages.size(); i++) {
			Rcout << m.messages[i] << std::endl;
		}
	}

	int nr = m.out.size();
	int nc = m.out[0].size();
	NumericMatrix mat(nr, nc);

	for (int i = 0; i < nr; i++) {
		for (int j = 0; j < nc; j++) {
			mat(i, j) = m.out[i][j];
		}
	}

	CharacterVector cnames(nc);
	for (int j = 0; j < nc; j++) {
		cnames[j] = m.out_names[j];
	}
	colnames(mat) = cnames;
	return(mat);
}
