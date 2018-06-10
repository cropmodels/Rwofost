/*
Author Robert Hijmans
Date: May 2016
License: GNU General Public License (GNU GPL) v. 2 
*/

#include <Rcpp.h>
using namespace Rcpp;
using namespace std;
#include "R_interface_util.h"
#include "wofost.h"


void setWeather(WofostModel* m, NumericVector date, NumericVector tmin, NumericVector tmax, NumericVector srad, NumericVector prec, NumericVector wind, NumericVector vapr, double latitude, double AngA, double AngB) {
	DailyWeather wth;
	wth.tmin = Rcpp::as<std::vector<double>>(tmin);
	wth.tmax = Rcpp::as<std::vector<double>>(tmax);
	wth.srad = Rcpp::as<std::vector<double>>(srad);
	wth.wind = Rcpp::as<std::vector<double>>(wind);
	wth.vapr = Rcpp::as<std::vector<double>>(vapr);
	wth.prec = Rcpp::as<std::vector<double>>(prec);
	wth.date = Rcpp::as<std::vector<long>>(date);
	wth.latitude  = latitude;
	wth.AngstromA = AngA;
	wth.AngstromB = AngB;
//	wth.elevation = location[2];
	m->wth = wth;
}





RCPP_EXPOSED_CLASS(DailyWeather)

RCPP_EXPOSED_CLASS(WofostCrop)
RCPP_EXPOSED_CLASS(WofostCropParameters)
RCPP_EXPOSED_CLASS(WofostCropParametersNPK)

RCPP_EXPOSED_CLASS(WofostSoil)
RCPP_EXPOSED_CLASS(WofostSoilParameters)
RCPP_EXPOSED_CLASS(WofostSoilParametersNPK)

RCPP_EXPOSED_CLASS(WofostControl)
RCPP_EXPOSED_CLASS(WofostModel)
RCPP_EXPOSED_CLASS(WofostOutput)


RCPP_MODULE(wofost){
    using namespace Rcpp;

    class_<WofostControl>("WofostControl")
		.field("modelstart", &WofostControl::modelstart) 
		.field("cropstart", &WofostControl::cropstart)  
		//.field("long_output", &WofostControl::long_output
		.field("IWB", &WofostControl::IWB)
		.field("IOXWL", &WofostControl::IOXWL)
		.field("ISTCHO", &WofostControl::ISTCHO) 
		.field("IDESOW", &WofostControl::IDESOW) 
		.field("IDLSOW", &WofostControl::IDLSOW) 
		.field("IENCHO", &WofostControl::IENCHO) 
		.field("IDAYEN", &WofostControl::IDAYEN) 
		.field("IDURMX", &WofostControl::IDURMX) 
	;

    class_<DailyWeather>("DailyWeather")
		.field("latitude", &DailyWeather::latitude) 
		.field("CO2",  &DailyWeather::CO2) 
		.field("date", &DailyWeather::date) 
		.field("srad", &DailyWeather::srad) 
		.field("tmin", &DailyWeather::tmin) 
		.field("tmax", &DailyWeather::tmax) 
		.field("prec", &DailyWeather::prec) 
		.field("wind", &DailyWeather::wind) 
		.field("vapr", &DailyWeather::vapr) 
	;
	
	// incomplete
    class_<WofostCropParametersNPK>("WofostCropParametersNPK")
		.field("TCNT", &WofostCropParametersNPK::TCNT)
		.field("TCPT", &WofostCropParametersNPK::TCPT)
		.field("TCKT", &WofostCropParametersNPK::TCKT)
	;

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
		.field("DVSI", &WofostCropParameters::DVSI)
		.field("DVSEND", &WofostCropParameters::DVSEND)
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
		.field("KDifTB", &WofostCropParameters::KDifTB)
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
		.field("pn", &WofostCrop::pn, "crop nutrient parameters")
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
		.field("SPADS", &WofostSoilParameters::SPADS)
		.field("SPASS", &WofostSoilParameters::SPASS)
		.field("SPODS", &WofostSoilParameters::SPODS)
		.field("SPOSS", &WofostSoilParameters::SPOSS)
		.field("DEFLIM", &WofostSoilParameters::DEFLIM)
		.field("IZT", &WofostSoilParameters::IZT)  // groundwater present
		.field("ifUNRN", &WofostSoilParameters::ifUNRN)
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
	
	
    class_<WofostSoilParametersNPK>("WofostSoilParametersNPK")
		.field("BG_N_SUPPLY", &WofostSoilParametersNPK::BG_N_SUPPLY)
	;
	
    class_<WofostSoil>("WofostSoil")
		.field("p", &WofostSoil::p, "soil parameters")
		.field("pn", &WofostSoil::pn, "soil nutrient parameters")
	;
	
    class_<WofostModel>("WofostModel")
		.constructor()
		.method("run", &WofostModel::model_run, "run the model")		
		.method("setWeather", &setWeather)
		.field("crop", &WofostModel::crop, "crop")
		.field("soil", &WofostModel::soil, "soil")
		.field("control", &WofostModel::control, "control")
		.field("out", &WofostModel::out, "out")
		.field("weather", &WofostModel::wth, "weather")
		
	;			

 /*   class_<WofostOutput>("WofostOutput")
		.field_readonly("LAI", &WofostOutput::LAI)
		.field_readonly("WLV", &WofostOutput::WLV)
		.field_readonly("WST", &WofostOutput::WST)
		.field_readonly("WRT", &WofostOutput::WRT)
		.field_readonly("WSO", &WofostOutput::WSO)
	;
*/	
};

	


