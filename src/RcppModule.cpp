/*
Author Robert Hijmans
Date: May 2016
License: GNU General Public License (GNU GPL) v. 2 
*/

#include <Rcpp.h>
#include "wofost.h"


void setWeather(WofostModel* m, Rcpp::NumericVector date, Rcpp::NumericVector tmin, Rcpp::NumericVector tmax, Rcpp::NumericVector srad, Rcpp::NumericVector prec, Rcpp::NumericVector wind, Rcpp::NumericVector vapr) {
	WofostWeather wth;
	wth.tmin = Rcpp::as<std::vector<double>>(tmin);
	wth.tmax = Rcpp::as<std::vector<double>>(tmax);
	wth.srad = Rcpp::as<std::vector<double>>(srad);
	wth.wind = Rcpp::as<std::vector<double>>(wind);
	wth.vapr = Rcpp::as<std::vector<double>>(vapr);
	wth.prec = Rcpp::as<std::vector<double>>(prec);
	wth.date = Rcpp::as<std::vector<long>>(date);
	m->wth = wth;
}



RCPP_EXPOSED_CLASS(WofostWeather)

RCPP_EXPOSED_CLASS(WofostCrop)
RCPP_EXPOSED_CLASS(WofostCropParameters)
//RCPP_EXPOSED_CLASS(WofostCropParametersNPK)

RCPP_EXPOSED_CLASS(WofostSoil)
RCPP_EXPOSED_CLASS(WofostSoilParameters)
//RCPP_EXPOSED_CLASS(WofostSoilParametersNPK)

RCPP_EXPOSED_CLASS(WofostControl)
RCPP_EXPOSED_CLASS(WofostModel)
RCPP_EXPOSED_CLASS(WofostOutput)
RCPP_EXPOSED_CLASS(WofostForcer)


RCPP_MODULE(wofost){
    using namespace Rcpp;

    class_<WofostControl>("WofostControl")
		.field("modelstart", &WofostControl::modelstart) 
		.field("cropstart", &WofostControl::cropstart)  
		.field("output", &WofostControl::output_option)
		.field("latitude", &WofostControl::latitude) 
		.field("elevation", &WofostControl::elevation) 
		.field("CO2",  &WofostControl::CO2) 
		.field("water_limited", &WofostControl::water_limited)
		//.field("nutrient_limited", &WofostControl::nutrient_limited)
		.field("watlim_oxygen", &WofostControl::IOXWL)
		
		.field("start_sowing", &WofostControl::ISTCHO) 
		//.field("IDESOW", &WofostControl::IDESOW) 
		//.field("IDLSOW", &WofostControl::IDLSOW) 
		//.field("IENCHO", &WofostControl::IENCHO) 
		//.field("IDAYEN", &WofostControl::IDAYEN) 
		.field("stop_maturity", &WofostControl::stop_maturity)
		.field("max_duration", &WofostControl::IDURMX) 
		.field("ANGSTA",  &WofostControl::ANGSTA) 
		.field("ANGSTB",  &WofostControl::ANGSTB) 
		//.field("usePENMAN",  &WofostControl::usePENMAN) 
		.field("useForce",  &WofostControl::useForce) 
	;

	
    class_<WofostWeather>("WofostWeather")
		.constructor()
		.field("date", &WofostWeather::date) 
		.field("srad", &WofostWeather::srad) 
		.field("tmin", &WofostWeather::tmin) 
		.field("tmax", &WofostWeather::tmax) 
		.field("prec", &WofostWeather::prec) 
		.field("wind", &WofostWeather::wind) 
		.field("vapr", &WofostWeather::vapr) 
	;
	
	/*
	// incomplete
    class_<WofostCropParametersNPK>("WofostCropParametersNPK")
		.field("TCNT", &WofostCropParametersNPK::TCNT)
		.field("TCPT", &WofostCropParametersNPK::TCPT)
		.field("TCKT", &WofostCropParametersNPK::TCKT)
	;
	*/
	
	class_<WofostCropParameters>("WofostCropParameters")
		.field("TBASEM", &WofostCropParameters::TBASEM)
		.field("TEFFMX", &WofostCropParameters::TEFFMX)
		.field("TSUMEM", &WofostCropParameters::TSUMEM)
		.field("IDSL", &WofostCropParameters::IDSL)
		.field("DLO", &WofostCropParameters::DLO)
		.field("DLC", &WofostCropParameters::DLC)
		.field("TSUM1", &WofostCropParameters::TSUM1)
		.field("TSUM2", &WofostCropParameters::TSUM2)
		.field("DTSMTB", &WofostCropParameters::DTSMTB)
		//.field("DVSI", &WofostCropParameters::DVSI)
		//.field("DVSEND", &WofostCropParameters::DVSEND)
		.field("TDWI", &WofostCropParameters::TDWI)
		.field("LAIEM", &WofostCropParameters::LAIEM)
		.field("RGRLAI", &WofostCropParameters::RGRLAI)
		.field("SLATB", &WofostCropParameters::SLATB)
		.field("SPA", &WofostCropParameters::SPA)
		.field("SSATB", &WofostCropParameters::SSATB)
		.field("SPAN", &WofostCropParameters::SPAN)
		.field("TBASE", &WofostCropParameters::TBASE)
		.field("CVL", &WofostCropParameters::CVL)
		.field("CVO", &WofostCropParameters::CVO)
		.field("CVR", &WofostCropParameters::CVR)
		.field("CVS", &WofostCropParameters::CVS)
		.field("Q10", &WofostCropParameters::Q10)
		.field("RML", &WofostCropParameters::RML)
		.field("RMO", &WofostCropParameters::RMO)
		.field("RMR", &WofostCropParameters::RMR)
		.field("RMS", &WofostCropParameters::RMS)
		.field("RFSETB", &WofostCropParameters::RFSETB)
		.field("FRTB", &WofostCropParameters::FRTB)
		.field("FLTB", &WofostCropParameters::FLTB)
		.field("FSTB", &WofostCropParameters::FSTB)
		.field("FOTB", &WofostCropParameters::FOTB)
		.field("PERDL", &WofostCropParameters::PERDL)
		.field("RDRRTB", &WofostCropParameters::RDRRTB)
		.field("RDRSTB", &WofostCropParameters::RDRSTB)
		.field("CFET", &WofostCropParameters::CFET)
		.field("DEPNR", &WofostCropParameters::DEPNR)
		.field("RDI", &WofostCropParameters::RDI)
		.field("RRI", &WofostCropParameters::RRI)
		.field("RDMCR", &WofostCropParameters::RDMCR)
		.field("IAIRDU", &WofostCropParameters::IAIRDU)
		.field("KDIFTB", &WofostCropParameters::KDIFTB)
		.field("EFFTB", &WofostCropParameters::EFFTB)
		.field("AMAXTB", &WofostCropParameters::AMAXTB)
		.field("TMPFTB", &WofostCropParameters::TMPFTB)
		.field("TMNFTB", &WofostCropParameters::TMNFTB)
		.field("CO2AMAXTB", &WofostCropParameters::CO2AMAXTB)
		.field("CO2EFFTB", &WofostCropParameters::CO2EFFTB)
		.field("CO2TRATB", &WofostCropParameters::CO2TRATB)
	;

	
    class_<WofostCrop>("WofostCrop")
		.field("p", &WofostCrop::p, "crop parameters")
		//.field("pn", &WofostCrop::pn, "crop nutrient parameters")
	;

	
    class_<WofostSoilParameters>("WofostSoilParameters")

		.field("SMTAB", &WofostSoilParameters::SMTAB)
		.field("SMW", &WofostSoilParameters::SMW)
		.field("SMFCF", &WofostSoilParameters::SMFCF)
		.field("SM0", &WofostSoilParameters::SM0)
		.field("CRAIRC", &WofostSoilParameters::CRAIRC)
		.field("CONTAB", &WofostSoilParameters::CONTAB)
		.field("K0", &WofostSoilParameters::K0)
		.field("SOPE", &WofostSoilParameters::SOPE)
		.field("KSUB", &WofostSoilParameters::KSUB)
		//.field("SPADS", &WofostSoilParameters::SPADS)
		//.field("SPASS", &WofostSoilParameters::SPASS)
		//.field("SPODS", &WofostSoilParameters::SPODS)
		//.field("SPOSS", &WofostSoilParameters::SPOSS)
		//.field("DEFLIM", &WofostSoilParameters::DEFLIM)
		.field("IZT", &WofostSoilParameters::IZT)  // groundwater present
		.field("IFUNRN", &WofostSoilParameters::IFUNRN)
		.field("WAV", &WofostSoilParameters::WAV)
		.field("ZTI", &WofostSoilParameters::ZTI)
		.field("DD", &WofostSoilParameters::DD)
		.field("IDRAIN", &WofostSoilParameters::IDRAIN) // presence of drains
		.field("NOTINF", &WofostSoilParameters::NOTINF) // fraction not inflitrating rainfall
		.field("SSMAX", &WofostSoilParameters::SSMAX) // max surface storage
		.field("SMLIM", &WofostSoilParameters::SMLIM)
		.field("SSI", &WofostSoilParameters::SSI)
		.field("RDMSOL", &WofostSoilParameters::RDMSOL)
	;
	
	/*	
    class_<WofostSoilParametersNPK>("WofostSoilParametersNPK")
		.field("BG_N_SUPPLY", &WofostSoilParametersNPK::BG_N_SUPPLY)
	;
	*/
	
    class_<WofostSoil>("WofostSoil")
		.field("p", &WofostSoil::p, "soil parameters")
		//.field("pn", &WofostSoil::pn, "soil nutrient parameters")
	;
	
    class_<WofostOutput>("WofostOutput")
		.field("names", &WofostOutput::names, "names")
		.field("values", &WofostOutput::values, "values")
	;

    class_<WofostForcer>("WofostForcer")
		.field("force_DVS", &WofostForcer::force_DVS, "force_DVS")
		.field("force_LAI", &WofostForcer::force_LAI, "force_LAI")
		.field("force_PAI", &WofostForcer::force_PAI, "force_LAI")
		.field("force_SAI", &WofostForcer::force_SAI, "force_LAI")
		.field("force_SM" , &WofostForcer::force_SM , "force_SM")
		.field("force_DMI", &WofostForcer::force_DMI, "force_DMI")
		.field("force_ADMI", &WofostForcer::force_ADMI, "force_ADMI")
		.field("force_RFTRA", &WofostForcer::force_RFTRA, "force_RFTRA")
		.field("force_FR" , &WofostForcer::force_FR , "force_FR")
		.field("force_FL" , &WofostForcer::force_FL , "force_FR")
		.field("force_WRT", &WofostForcer::force_WRT, "force_WRT")
		.field("force_WLV", &WofostForcer::force_WLV, "force_WLV")
		.field("force_WST", &WofostForcer::force_WST, "force_WST")
		.field("force_WSO", &WofostForcer::force_WSO, "force_WSO")
		.field("DVS", &WofostForcer::DVS, "DVS")
		.field("LAI", &WofostForcer::LAI, "LAI")
		.field("PAI", &WofostForcer::PAI, "SAI")
		.field("SAI", &WofostForcer::SAI, "PAI")
		.field("SM", &WofostForcer::SM, "SM")
		.field("ADMI", &WofostForcer::ADMI, "ADMI")
		.field("RFTRA", &WofostForcer::RFTRA, "RFTRA")
		.field("DMI", &WofostForcer::DMI, "DMI")
		.field("FR", &WofostForcer::FR, "FR")
		.field("FL", &WofostForcer::FL, "FL")
		.field("WRT", &WofostForcer::WRT, "WRT")
		.field("WLV", &WofostForcer::WLV, "WLV")
		.field("WST", &WofostForcer::WST, "WST")
		.field("WSO", &WofostForcer::WSO, "WSO")
	;

    class_<WofostModel>("WofostModel")
		.constructor()
		.method("run", &WofostModel::model_run, "run the model")		
		//.method("setWeather", &setWeather)
		.field("crop", &WofostModel::crop, "crop")
		.field("soil", &WofostModel::soil, "soil")
		.field("control", &WofostModel::control, "control")
		.field("weather", &WofostModel::wth, "weather")
		.field("output", &WofostModel::output, "output")
		.field("forcer", &WofostModel::forcer, "forcer")
		.field("messages", &WofostModel::messages, "messages")
		.field("fatalError", &WofostModel::fatalError, "fatalError")
	;			

};

	


