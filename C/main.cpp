#include <vector>
#include <fstream>
#include "wofost.h"
#include "date.h"
#include "files.h"


int date2int (date x) {
	date s(1970, 1, 1);
	return x - s;
}

// crop parameters
WofostCrop getCropParameters(const char *filename) {
	std::vector<std::vector<std::string> > crop = readINI(filename);

	WofostCrop crp;

	crp.p.TBASEM = dFromINI(crop, "TBASEM");
	crp.p.TEFFMX = dFromINI(crop, "TEFFMX");
	crp.p.TSUMEM = dFromINI(crop, "TSUMEM");
	crp.p.IDSL = iFromINI(crop, "IDSL");
	crp.p.DLO = dFromINI(crop, "DLO");
	crp.p.DLC = dFromINI(crop, "DLC");
	crp.p.TSUM1 = dFromINI(crop, "TSUM1");
	crp.p.TSUM2 = dFromINI(crop, "TSUM2");
	crp.p.DTSMTB = dvFromINI(crop, "DTSMTB");

	crp.p.DVSI = dFromINI(crop, "DVSI");
	crp.p.DVSEND = dFromINI(crop, "DVSEND");
	crp.p.TDWI = dFromINI(crop, "TDWI");
	crp.p.LAIEM = dFromINI(crop, "LAIEM");
	crp.p.RGRLAI = dFromINI(crop, "RGRLAI");
	crp.p.SLATB = dvFromINI(crop, "SLATB");
	crp.p.SPA = dFromINI(crop, "SPA");
	crp.p.SSATB = dvFromINI(crop, "SSATB");
	crp.p.SPAN = dFromINI(crop, "SPAN");
	crp.p.TBASE = dFromINI(crop, "TBASE");
	crp.p.CVL = dFromINI(crop, "CVL");
	crp.p.CVO = dFromINI(crop, "CVO");
	crp.p.CVR = dFromINI(crop, "CVR");
	crp.p.CVS = dFromINI(crop, "CVS");
	crp.p.Q10 = dFromINI(crop, "Q10");
	crp.p.RML = dFromINI(crop, "RML");
	crp.p.RMO = dFromINI(crop, "RMO");
	crp.p.RMR = dFromINI(crop, "RMR");
	crp.p.RMS = dFromINI(crop, "RMS");
	crp.p.RFSETB = dvFromINI(crop, "RFSETB");
	crp.p.FRTB = dvFromINI(crop, "FRTB");
	crp.p.FLTB = dvFromINI(crop, "FLTB");
	crp.p.FSTB = dvFromINI(crop, "FSTB");
	crp.p.FOTB = dvFromINI(crop, "FOTB");

	crp.p.PERDL = dFromINI(crop, "PERDL");
	crp.p.RDRRTB = dvFromINI(crop, "RDRRTB");
	crp.p.RDRSTB = dvFromINI(crop, "RDRSTB");
	crp.p.CFET = dFromINI(crop, "CFET");
	crp.p.DEPNR = dFromINI(crop, "DEPNR");
	crp.p.RDI = dFromINI(crop, "RDI");
	crp.p.RRI = dFromINI(crop, "RRI");
	crp.p.RDMCR = dFromINI(crop, "RDMCR");

	crp.p.IAIRDU = iFromINI(crop, "IAIRDU");

	crp.p.KDIFTB = dvFromINI(crop, "KDIFTB");
	crp.p.EFFTB = dvFromINI(crop, "EFFTB");
	crp.p.AMAXTB = dvFromINI(crop, "AMAXTB");
	crp.p.TMPFTB = dvFromINI(crop, "TMPFTB");
	crp.p.TMNFTB = dvFromINI(crop, "TMNFTB");

	crp.p.CO2AMAXTB = dvFromINI(crop, "CO2AMAXTB");
	crp.p.CO2EFFTB = dvFromINI(crop, "CO2EFFTB");
	crp.p.CO2TRATB = dvFromINI(crop, "CO2TRATB");


	return crp;
}

// soil parameters
WofostSoil getSoilParameters(const char *filename) {
	std::vector<std::vector<std::string> > soil = readINI(filename);

	WofostSoil sol;

	sol.p.SMTAB = dvFromINI(soil, "SMTAB");
	sol.p.SMW = dFromINI(soil, "SMW");
	sol.p.SMFCF = dFromINI(soil, "SMFCF");
	sol.p.SM0 = dFromINI(soil, "SM0");
	sol.p.CRAIRC = dFromINI(soil, "CRAIRC");
	sol.p.CONTAB = dvFromINI(soil, "CONTAB");
	sol.p.K0 = dFromINI(soil, "K0");
	sol.p.SOPE = dFromINI(soil, "SOPE");
	sol.p.KSUB = dFromINI(soil, "KSUB");
	sol.p.SPADS = dFromINI(soil, "SPADS");
	sol.p.SPASS = dFromINI(soil, "SPASS");
	sol.p.SPODS = dFromINI(soil, "SPODS");
	sol.p.SPOSS = dFromINI(soil, "SPOSS");
	sol.p.DEFLIM = dFromINI(soil, "DEFLIM");

	//soil variables but in control
	sol.p.IZT = iFromINI(soil, "IZT");  // groundwater present
	sol.p.IFUNRN = iFromINI(soil, "IFUNRN");
	sol.p.WAV = dFromINI(soil, "WAV");
	sol.p.ZTI = dFromINI(soil, "ZTI");
	sol.p.DD = dFromINI(soil, "DD");
	sol.p.RDMSOL = dFromINI(soil, "RDMSOL");

	//soil variables but in control, but that's OK -- makes sense
	sol.p.IDRAIN = iFromINI(soil, "IDRAIN"); // presence of drains
	sol.p.NOTINF = iFromINI(soil, "NOTINF"); // fraction not inflitrating rainfall
	sol.p.SSMAX = dFromINI(soil, "SSMAX"); // max surface storage
	sol.p.SMLIM = dFromINI(soil, "SMLIM");
	sol.p.SSI = dFromINI(soil, "SSI");

	return sol;
}


// weather
WofostWeather getWeatherParameters(const char *filename) {

	std::vector< std::vector<std::string> > matrix = readCSV(filename);

	WofostWeather wth;
	date dates;

	std::vector<std::string> ss;
	for (size_t i = 1; i < matrix.size(); i++) { // skips first line of file

		ss = split(matrix[i][0], '-');
		dates.set_year( std::stoi(ss[0].c_str()) );
		dates.set_month( std::stoi(ss[1].c_str()) );
		dates.set_day( std::stoi(ss[2].c_str()) );
		int intdate = date2int(dates);
		wth.date.push_back( intdate );
		wth.srad.push_back( std::stod(matrix[i][1]) );
		wth.tmin.push_back( std::stod(matrix[i][2]) );
		wth.tmax.push_back( std::stod(matrix[i][3]) );
		wth.vapr.push_back( std::stod(matrix[i][4]) );
		wth.wind.push_back( std::stod(matrix[i][5]) );
		wth.prec.push_back( std::stod(matrix[i][6]) );
	} // ordering should not matter

	return wth;
}

// control parameters
WofostControl getControlParameters(const char *filename) {
	WofostControl tim;
	std::vector<std::vector<std::string> > control = readINI(filename);
	date start = dateFromINI(control, "modelstart");
    tim.modelstart = date2int(start);
    tim.cropstart = iFromINI(control, "cropstart");
    tim.output_option = sFromINI(control, "output", "default");
	tim.water_limited = bFromINI(control, "water_limited");
	tim.IOXWL  = iFromINI(control, "IOXWL");
	tim.IENCHO = iFromINI(control, "IENCHO");
	tim.IDAYEN = iFromINI(control, "IDAYEN");
	tim.IDURMX = iFromINI(control, "IDURMX");
//	date emergence = dateFromINI(control, "emergence") );
	tim.latitude  = dFromINI(control, "latitude");
	tim.elevation = dFromINI(control, "elevation");
	tim.CO2       = dFromINI(control, "CO2");
	tim.ANGSTA = dFromINI(control, "ANGSTA", -0.18);
	tim.ANGSTB = dFromINI(control, "ANGSTB", -0.55);

	return tim;
}



std::vector<std::string> getFiles(char *filename)  {
	std::vector<std::vector<std::string> > ini = readINI(filename);
	std::vector<std::string> files;
	files.push_back( sFromINI(ini, "crop") );
	files.push_back( sFromINI(ini, "weather") );
	files.push_back( sFromINI(ini, "soil") );
	files.push_back( sFromINI(ini, "control") );
	files.push_back( sFromINI(ini, "output") );
	return files;
}

void writeOutput(const char *filename, WofostModel m) {
	std::ofstream outfile;
	outfile.open(filename);

	int nvar = m.output.names.size();
	std::string outnames = m.output.names[0];
	for(size_t i = 1; i < m.output.names.size(); i++){
		outnames = outnames + ',' + m.output.names[i];
	}
	outfile << outnames << std::endl;

	size_t nrow = (m.output.values.size() / nvar);
	std::cout << nrow << " rows written to: " << filename << std::endl;
	std::cout << outnames << std::endl;
    size_t k=0;
	for (size_t i = 0; i < nrow; i++) {
		std::string s = std::to_string(m.output.values[k]);
		k++;
		for (int j = 1; j < nvar; j++) {
			s = s + ',' + std::to_string(m.output.values[k]);
			k++;
		}
		outfile << s << std::endl;
		if (i < 5)	std::cout << s << std::endl;
	}
	outfile.close();
}

int main(int argc, char *argv[]) {

    char *inputFile;
    if(argc < 2) {
        inputFile = (char *)"input.ini";
            //std::string path = (std::string)argv[0];
            //std::string basename = path.substr(path.find_last_of("/\\") + 1);
            //std::string usage = "Usage: " + basename + " input.ini";
            //std::cout << usage << std::endl;
            //return 1;
    } else {
        inputFile = argv[1];
    }
	std::vector<std::string> files = getFiles(inputFile);
	const char *cropFile = files[0].c_str();
	const char *weatherFile = files[1].c_str();
	const char *soilFile = files[2].c_str();
	const char *controlFile = files[3].c_str();
	const char *outputFile = files[4].c_str();

	WofostCrop crp = getCropParameters(cropFile);
	WofostSoil sol = getSoilParameters(soilFile);
	WofostControl tim = getControlParameters(controlFile);
	WofostWeather wth = getWeatherParameters(weatherFile);

//	unsigned int nwth = wth.tmin.size();

	WofostModel m;
	m.crop = crp;
	m.soil = sol;
	m.control = tim;
    //int start = date2int(date(1977, 1, 1));
	//m.control.modelstart = start;
	m.wth = wth;
//    m.wth$latitude <- 52.57
	m.model_run();
    for (size_t i=0; i<m.messages.size(); i++) {
       std::cout << m.messages[i] << std::endl;
    }
	writeOutput(outputFile, m);

	return 0;
}

