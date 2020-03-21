/*
Authors: Robert Hijmans and Huang Fang
Date: June 2016

License: GNU General Public License (GNU GPL) v. 2
*/

#include <Rcpp.h>
using namespace Rcpp;
#include "SimUtil.h"
#include "wofost.h"

template <class T>
T valueFromList(List lst, const char*s) {
	if (!lst.containsElementNamed(s) ) {
		std::string ss = "parameter '" +  std::string(s) + "' not found";
		stop(ss);
	}
	T v = lst[s];
	return(v);
}

template <class T>
T valueFromListDefault(List lst, const char*s, T def) {
	if (!lst.containsElementNamed(s) ) {
		return def;
	}
	T v = lst[s];
	return(v);
}


template <class T>
std::vector<T> vectorFromList(List lst, const char*s) {
	if (!lst.containsElementNamed(s) ) {
		std::string ss = "parameter '" +  std::string(s) + "' not found";
		stop(ss);
	}
	std::vector<T> v = lst[s];
	return(v);
}


template <class T>
std::vector<T> vectorFromDF(DataFrame d, std::string s) {
	CharacterVector nms = d.names();
	auto it = std::find(nms.begin(), nms.end(), s);
	unsigned pos = std::distance(nms.begin(), it);
	if (pos == nms.size()) {
		std::string ss = "Variable '" + s + "' not found";
		stop(ss);
	}
	std::vector<T> v = d[pos];
	return(v);
}


std::vector<double> TableFromList(List lst, const char* s){
	if(! lst.containsElementNamed(s)){
		std::string ss = "parameter '" +  std::string(s) + "' not found";
		stop(ss);
	}

	NumericMatrix x = lst[s];
	if(x.nrow() != 2){
		std::string ss2 = "nrow != 2";
		stop(ss2);
	}
	//std::vector<double> result;
	//result.insert(result.end(), x.begin(), x.end());
	std::vector<double> result = Rcpp::as<std::vector<double> >(x);
	return result;
}



// [[Rcpp::export(".wofost")]]
NumericMatrix wofost(List crop, DataFrame weather, List soil, List control) {

// control ("timer") parameters
	struct WofostControl cntr;
	struct WofostCrop crp;

	cntr.modelstart = valueFromList<long>(control, "modelstart");
	cntr.cropstart = valueFromList<unsigned>(control, "cropstart");
	
	cntr.output_option = valueFromListDefault<std::string>(control, "output", "");
	
	cntr.latitude  = valueFromList<double>(control, "latitude");
	cntr.elevation = valueFromList<double>(control, "elevation");
	cntr.CO2 = valueFromListDefault<double>(control, "CO2", 360);
	cntr.ANGSTA = valueFromListDefault<double>(control, "ANGSTA", 0.18);
	cntr.ANGSTB = valueFromListDefault<double>(control, "ANGSTB", 0.55);
	
	cntr.water_limited = valueFromListDefault<bool>(control, "water_limited", false); 
	//cntr.nutrient_limited = valueFromListDefault<bool>(control, "nutrient_limited", false); 
	cntr.IOXWL = valueFromList<int>(control, "watlim_oxygen");

	cntr.ISTCHO = valueFromList<int>(control, "start_sowing");
	//cntr.IDESOW = valueFromList<int>(control, "IDESOW");
	//cntr.IDLSOW = valueFromList<int>(control, "IDLSOW");

	//cntr.IENCHO = valueFromList<int>(control, "IENCHO");
	//cntr.IDAYEN = valueFromList<int>(control, "IDAYEN");
	cntr.IDURMX = valueFromList<int>(control, "max_duration");
	cntr.stop_maturity = valueFromList<int>(control, "stop_maturity");
	//npk
	/*
	cntr.npk_model = valueFromListDefault<int>(control, "npk_model", false);
	if(cntr.npk_model){
		cntr.NPKdates = vectorFromList<long>(control, "NPKdates");
		cntr.N_amount = vectorFromList<double>(control, "N");
		cntr.P_amount = vectorFromList<double>(control, "P");
		cntr.K_amount = vectorFromList<double>(control, "K");

		crp.pn.TCNT = valueFromList<double>(crop, "TCNT");
		crp.pn.TCPT = valueFromList<double>(crop, "TCPT");
		crp.pn.TCKT = valueFromList<double>(crop, "TCKT");
		crp.pn.DVSNPK_STOP = valueFromList<double>(crop, "DVSNPK_STOP");
		crp.pn.NFIX_FR = valueFromList<double>(crop, "NFIX_FR");
		crp.pn.NPART = valueFromList<double>(crop, "NPART");
		crp.pn.NMAXSO = valueFromList<double>(crop, "NMAXSO");
		crp.pn.PMAXSO = valueFromList<double>(crop, "PMAXSO");
		crp.pn.KMAXSO = valueFromList<double>(crop, "KMAXSO");
		crp.pn.NMAXRT_FR = valueFromList<double>(crop, "NMAXRT_FR");
		crp.pn.PMAXRT_FR = valueFromList<double>(crop, "PMAXRT_FR");
		crp.pn.KMAXRT_FR = valueFromList<double>(crop, "KMAXRT_FR");
		crp.pn.NMAXST_FR = valueFromList<double>(crop, "NMAXST_FR");
		crp.pn.PMAXST_FR = valueFromList<double>(crop, "PMAXST_FR");
		crp.pn.KMAXST_FR = valueFromList<double>(crop, "KMAXST_FR");
		crp.pn.NRESIDLV = valueFromList<double>(crop, "NRESIDLV");
		crp.pn.NRESIDST = valueFromList<double>(crop, "NRESIDST");
		crp.pn.NRESIDRT = valueFromList<double>(crop, "NRESIDRT");
		crp.pn.PRESIDLV = valueFromList<double>(crop, "PRESIDLV");
		crp.pn.PRESIDST = valueFromList<double>(crop, "PRESIDST");
		crp.pn.PRESIDRT = valueFromList<double>(crop, "PRESIDRT");
		crp.pn.KRESIDLV = valueFromList<double>(crop, "KRESIDLV");
		crp.pn.KRESIDST = valueFromList<double>(crop, "KRESIDST");
		crp.pn.KRESIDRT = valueFromList<double>(crop, "KRESIDRT");
		crp.pn.NCRIT_FR = valueFromList<double>(crop, "NCRIT_FR");
		crp.pn.PCRIT_FR = valueFromList<double>(crop, "PCRIT_FR");
		crp.pn.KCRIT_FR = valueFromList<double>(crop, "KCRIT_FR");
		crp.pn.NLUE_NPK = valueFromList<double>(crop, "NLUE_NPK");
		crp.pn.NPK_TRANSLRT_FR = valueFromList<double>(crop, "NPK_TRANSLRT_FR");
		crp.pn.NMAXLV_TB = TableFromList(crop, "NMAXLV_TB");
		crp.pn.PMAXLV_TB = TableFromList(crop, "PMAXLV_TB");
		crp.pn.KMAXLV_TB = TableFromList(crop, "KMAXLV_TB");
	}
	*/
	
	crp.p.TBASEM = valueFromList<double>(crop, "TBASEM");
	crp.p.TEFFMX = valueFromList<double>(crop, "TEFFMX");
	crp.p.TSUMEM = valueFromList<double>(crop, "TSUMEM");
	crp.p.IDSL = valueFromList<int>(crop, "IDSL");
	crp.p.DLO = valueFromList<double>(crop, "DLO");
	crp.p.DLC = valueFromList<double>(crop, "DLC");
	crp.p.TSUM1 = valueFromList<double>(crop, "TSUM1");
	crp.p.TSUM2 = valueFromList<double>(crop, "TSUM2");
	crp.p.DTSMTB = TableFromList(crop, "DTSMTB");
	//crp.p.DVSI = valueFromList<double>(crop, "DVSI");
	//crp.p.DVSEND = valueFromList<double>(crop, "DVSEND");
	crp.p.TDWI = valueFromList<double>(crop, "TDWI");
	crp.p.LAIEM = valueFromList<double>(crop, "LAIEM");
	crp.p.RGRLAI = valueFromList<double>(crop, "RGRLAI");
	crp.p.SLATB = TableFromList(crop, "SLATB");
	crp.p.SPA = valueFromList<double>(crop, "SPA");
	crp.p.SSATB = TableFromList(crop, "SSATB");
	crp.p.SPAN = valueFromList<double>(crop, "SPAN");
	crp.p.TBASE = valueFromList<double>(crop, "TBASE");
	crp.p.CVL = valueFromList<double>(crop, "CVL");
	crp.p.CVO = valueFromList<double>(crop, "CVO");
	crp.p.CVR = valueFromList<double>(crop, "CVR");
	crp.p.CVS = valueFromList<double>(crop, "CVS");
	crp.p.Q10 = valueFromList<double>(crop, "Q10");
	crp.p.RML = valueFromList<double>(crop, "RML");
	crp.p.RMO = valueFromList<double>(crop, "RMO");
	crp.p.RMR = valueFromList<double>(crop, "RMR");
	crp.p.RMS = valueFromList<double>(crop, "RMS");
	crp.p.RFSETB = TableFromList(crop, "RFSETB");
	crp.p.FRTB = TableFromList(crop, "FRTB");
	crp.p.FLTB = TableFromList(crop, "FLTB");
	crp.p.FSTB = TableFromList(crop, "FSTB");
	crp.p.FOTB = TableFromList(crop, "FOTB");
	crp.p.PERDL = valueFromList<double>(crop, "PERDL");
	crp.p.RDRRTB = TableFromList(crop, "RDRRTB");
	crp.p.RDRSTB = TableFromList(crop, "RDRSTB");
	crp.p.CFET = valueFromList<double>(crop, "CFET");
	crp.p.DEPNR = valueFromList<double>(crop, "DEPNR");
	crp.p.RDI = valueFromList<double>(crop, "RDI");
	crp.p.RRI = valueFromList<double>(crop, "RRI");
	crp.p.RDMCR = valueFromList<double>(crop, "RDMCR");

	crp.p.IAIRDU = valueFromList<int>(crop, "IAIRDU");

	crp.p.KDIFTB = TableFromList(crop, "KDIFTB");
	crp.p.EFFTB = TableFromList(crop, "EFFTB");
	crp.p.AMAXTB = TableFromList(crop, "AMAXTB");
	crp.p.TMPFTB = TableFromList(crop, "TMPFTB");
	crp.p.TMNFTB = TableFromList(crop, "TMNFTB");

	crp.p.CO2AMAXTB = TableFromList(crop, "CO2AMAXTB");
	crp.p.CO2EFFTB = TableFromList(crop, "CO2EFFTB");
	crp.p.CO2TRATB = TableFromList(crop, "CO2TRATB");


// soil parameters
	struct WofostSoil sol;

	//if (cntr.IOXWL != 0) {
	
	sol.p.SMTAB = TableFromList(soil, "SMTAB");
	
	//} else { // should be no need to read it; need to check if true
	//	sol.p.SMTAB = TableFromList(soil, "SMTAB");
	//}
	sol.p.SMW = valueFromList<double>(soil, "SMW");
	sol.p.SMFCF = valueFromList<double>(soil, "SMFCF");
	sol.p.SM0 = valueFromList<double>(soil, "SM0");
	sol.p.CRAIRC = valueFromList<double>(soil, "CRAIRC");
	sol.p.CONTAB = TableFromList(soil, "CONTAB");
	sol.p.K0 = valueFromList<double>(soil, "K0");
	sol.p.SOPE = valueFromList<double>(soil, "SOPE");
	sol.p.KSUB = valueFromList<double>(soil, "KSUB");
	//sol.p.SPADS = valueFromList<double>(soil, "SPADS");
	//sol.p.SPASS = valueFromList<double>(soil, "SPASS");
	//sol.p.SPODS = valueFromList<double>(soil, "SPODS");
	//sol.p.SPOSS = valueFromList<double>(soil, "SPOSS");
	//sol.p.DEFLIM = valueFromList<double>(soil, "DEFLIM");


	//soil variables that used to be in the control object
	sol.p.IZT = valueFromList<int>(soil, "IZT");  // groundwater present
	sol.p.IFUNRN = valueFromList<int>(soil, "IFUNRN");
	sol.p.WAV = valueFromList<double>(soil, "WAV");
	sol.p.ZTI = valueFromList<double>(soil, "ZTI");
	sol.p.DD = valueFromList<double>(soil, "DD");
	sol.p.RDMSOL = valueFromList<double>(soil, "RDMSOL");

	sol.p.IDRAIN = valueFromList<int>(soil, "IDRAIN"); // presence of drains
	sol.p.NOTINF = valueFromList<int>(soil, "NOTINF"); // fraction not inflitrating rainfall
	sol.p.SSMAX = valueFromList<double>(soil, "SSMAX"); // max surface storage
	sol.p.SMLIM = valueFromList<double>(soil, "SMLIM");
	sol.p.SSI = valueFromList<double>(soil, "SSI");

	/*
	if(cntr.npk_model){
		sol.pn.N_recovery = vectorFromList<double>(control, "Nrecovery");
		sol.pn.P_recovery = vectorFromList<double>(control, "Precovery");
		sol.pn.K_recovery = vectorFromList<double>(control, "Krecovery");
		sol.pn.BG_N_SUPPLY = valueFromList<double>(soil, "BG_N_SUPPLY");
		sol.pn.BG_P_SUPPLY = valueFromList<double>(soil, "BG_P_SUPPLY");
		sol.pn.BG_K_SUPPLY = valueFromList<double>(soil, "BG_K_SUPPLY");
		sol.pn.NSOILBASE = valueFromList<double>(soil, "NSOILBASE");
		sol.pn.PSOILBASE = valueFromList<double>(soil, "PSOILBASE");
		sol.pn.KSOILBASE = valueFromList<double>(soil, "KSOILBASE");
		sol.pn.NSOILBASE_FR = valueFromList<double>(soil, "NSOILBASE_FR");
		sol.pn.PSOILBASE_FR = valueFromList<double>(soil, "PSOILBASE_FR");
		sol.pn.KSOILBASE_FR = valueFromList<double>(soil, "KSOILBASE_FR");

	}
	*/

// weather
	WofostWeather wth;
	wth.tmin = vectorFromDF<double>(weather, "tmin");
	wth.tmax = vectorFromDF<double>(weather, "tmax");
	wth.srad = vectorFromDF<double>(weather, "srad");
	wth.prec = vectorFromDF<double>(weather, "prec");
	wth.vapr = vectorFromDF<double>(weather, "vapr");
	wth.wind = vectorFromDF<double>(weather, "wind");
	wth.date = vectorFromDF<long>(weather, "date");
	
	WofostModel m;
	m.crop = crp;
	m.soil = sol;
	m.control = cntr;
	m.wth = wth;
	m.model_run();

// handle messages
	// std::vector<std::string>  messages;
	// bool fatalError;

	if (m.fatalError) {
		for (size_t i = 0; i < m.messages.size(); i++) {
			Rcout << m.messages[i] << std::endl;
		}
	}

	size_t nc = m.output.names.size();
	size_t nr = m.output.values.size() / nc;
	NumericMatrix mat(nr, nc);
	CharacterVector cnames = wrap(m.output.names);
	colnames(mat) = cnames;

	size_t k=0;
	for (size_t i = 0; i < nr; i++) {
		for (size_t j = 0; j < nc; j++) {
			mat(i, j) = m.output.values[k];
			k++;
		}
	}

	return(mat);
}

