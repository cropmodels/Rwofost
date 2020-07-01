/*
Robert Hijmans
June 2020
*/

#include <cmath>
#include <vector>
#include "wofost.h"

std::vector<double> WofostModel::run_batch(std::vector<double> tmin, std::vector<double> tmax, std::vector<double> srad, std::vector<double> prec, std::vector<double> vapr, std::vector<double> wind, std::vector<long> date, std::vector<long> mstart) {
	
	size_t sz = date.size();
	size_t n = tmin.size() / sz;
	size_t nsim = mstart.size();
	
	std::vector<double> out(n * nsim, NAN);
	control.output_option = "BATCH";
	wth.date = date;
	for (size_t i=0; i<n; i++) {
		size_t offset = sz * i;
		if (std::isnan(tmin[offset])) {
			continue;
		}
		wth.tmin = std::vector<double>(tmin.begin()+offset, tmin.begin()+offset+sz);
		wth.tmax = std::vector<double>(tmax.begin()+offset, tmax.begin()+offset+sz);
		wth.srad = std::vector<double>(srad.begin()+offset, srad.begin()+offset+sz);
		wth.vapr = std::vector<double>(vapr.begin()+offset, vapr.begin()+offset+sz);
		wth.wind = std::vector<double>(wind.begin()+offset, wind.begin()+offset+sz);
		wth.prec = std::vector<double>(prec.begin()+offset, prec.begin()+offset+sz);

		for (size_t j=0; j<nsim; j++) {
			control.modelstart = mstart[j];
			run();
			out[j*n +i] = output.values[output.values.size()-1];
		}
	}
	return out;
}

