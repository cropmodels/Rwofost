/*
Robert Hijmans
June 2020
*/

#include <cmath>
#include <vector>
#include "wofost.h"

#include "Rcpp.h"

std::vector<double> WofostModel::run_batch(std::vector<double> tmin, std::vector<double> tmax, std::vector<double> srad, std::vector<double> prec, std::vector<double> vapr, std::vector<double> wind, std::vector<long> date, std::vector<long> mstart, std::vector<int> soilindex, WofostSoilCollection soils, std::vector<double> depth) {


	bool watlim = control.water_limited;
	
	size_t sz = date.size(); // nlyr of input rasters
	size_t nc = tmin.size() / sz; // ncell of input rasters

	// number of simulations per cell
	size_t nsim = mstart.size();
	std::vector<double> out(nc * nsim, NAN);

	// nc = number of cells 
	if (watlim && (nc != soilindex.size())) {
		Rcpp::Rcout << "bad soil index data" << std::endl;
		return out;
	}
	int nsoils = soils.soils.size();
	if (nsoils == 0) {
		Rcpp::Rcout << "bad soil data" << std::endl;
		return out;
	}
	soil = soils.soils[0];
	control.output_option = "BATCH";
	wth.date = date;

	for (size_t i=0; i<nc; i++) {
		size_t offset = sz * i;
		if (std::isnan(tmin[offset])) {
			continue;
		}
		if (watlim) {
			double sidx = soilindex[i]-1;
			if ((sidx < 0) || sidx >= nsoils) {
				continue;
			} 
			soil = soils.soils[sidx-1];
			if (depth[i] >= 0) {
				soil.p.RDMSOL = depth[i];
			}
			wth.vapr = std::vector<double>(vapr.begin()+offset, vapr.begin()+offset+sz);
			wth.wind = std::vector<double>(wind.begin()+offset, wind.begin()+offset+sz);
			wth.prec = std::vector<double>(prec.begin()+offset, prec.begin()+offset+sz);
		}
		wth.tmin = std::vector<double>(tmin.begin()+offset, tmin.begin()+offset+sz);
		wth.tmax = std::vector<double>(tmax.begin()+offset, tmax.begin()+offset+sz);
		wth.srad = std::vector<double>(srad.begin()+offset, srad.begin()+offset+sz);
		
		for (size_t j=0; j<nsim; j++) {
			control.modelstart = mstart[j];
			run();
			double yield = output.values[output.values.size()-1];		
			out[j*nc+i] = yield;
		}
	}
	return out;
}


