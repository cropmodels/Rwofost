/*
Author: Robert Hijmans
2016-2020

License: GNU General Public License (GNU GPL) v. 2
*/

#include <vector>
#include <string>
#include "SimUtil.h"

class WofostWeather {
public:
	std::vector<long> date;
	std::vector<double> srad, tmin, tmax, prec, wind, vapr;
};


struct WofostControl {
	long modelstart;
	unsigned cropstart; // sowing, emergence, ...;
	//bool long_output;
	std::string output_option;
	//bool npk_model = false;  //if model is npk, default false

	double latitude, elevation;
	double CO2 = 360;
	
	//bool usePENMAN = true;
	double ANGSTA = 0.18;
	double ANGSTB = 0.55;

	unsigned IDESOW;
	int INYRG, ISTCHO, IDLSOW, IDURMX;
	//IENCHO, IDAYEN
	bool stop_maturity = true;
	bool IOXWL = false;  //oxygen limitation
	//int	IPRODL, IWB; // water limited (1) or potential (0)
	bool water_limited = false; 
	// nutrient_limited = false;
	//std::vector<double> N_amount, P_amount, K_amount;
	//std::vector<long> NPKdates;
	bool useForce = false;
};


/*
struct WofostCropParametersNPK {
	double TCNT, TCPT, TCKT;
	double DVSNPK_STOP, NFIX_FR, NPART;
	double NMAXSO, PMAXSO, KMAXSO;
	double NMAXRT_FR, PMAXRT_FR, KMAXRT_FR;
	double NMAXST_FR, PMAXST_FR, KMAXST_FR;
	std::vector<double> NMAXLV_TB, PMAXLV_TB, KMAXLV_TB;
	double NRESIDLV, NRESIDST, NRESIDRT, PRESIDLV, PRESIDST, PRESIDRT, KRESIDLV, KRESIDST, KRESIDRT;
	double NCRIT_FR, PCRIT_FR, KCRIT_FR;
	double NLUE_NPK, NPK_TRANSLRT_FR;
};
*/

struct WofostCropParameters {
    bool IAIRDU = false; // airducts present (true for rice)
	int IDSL;
	double DVSEND = 2; // now fixed
	double DVSI = 0;
	double DLO, DLC, TSUM1, TSUM2, TDWI, RGRLAI, SPA, SPAN, TBASE;
	double CVL, CVO, CVR, CVS, Q10, RML, RMO, RMR, RMS, PERDL, CFET, DEPNR, RDMCR, RRI, RDI;
	double LAIEM;
	//emergence parameters
	double TBASEM, TEFFMX, TSUMEM;
	//tables
	std::vector<double> DTSMTB, AMAXTB, TMPFTB, KDIFTB, EFFTB, TMNFTB, RFSETB, SLATB, FRTB, FLTB, FSTB, FOTB, RDRSTB, RDRRTB, SSATB;
	std::vector<double> CO2AMAXTB, CO2EFFTB, CO2TRATB;
	
	//vernalization
	unsigned VERNSAT; // Saturated vernalisation requirements (days)
	unsigned VERNBASE; // Base vernalisation requirements (days)
	std::vector<double> VERNRTB; // vernalisation rate temperature response.
	double VERNDVS; // DVS at which vernalisation is fulfilled
	
} ;

struct WofostCropRates {
// rates
	double GASS, GWST, GWSO;
	double DRST, DRLV, DRRT, GWRT, DRSO; // dead rates
	double DVR, DTSUME, DTSUM, GLAIEX;
	double RR, FYSDEL;
	
	double VERNR;   // Rate of vernalisation
    double VERNFAC; // Red. factor for phenol. devel.
};

struct WofostCropStates {
	double RD=0;
	double RDOLD, GRLV, DWRT, DWLV, DWST, DWSO;
	double DVS, LAI, LAIEXP, SAI, PAI, WRT, WLV, WST, WSO;
	double TWRT, TWLV, TWST, TWSO, TAGP, TSUM, TSUME, TADW;

	unsigned VERN; // Vernalisation state (days)
	long DOV;      // Day when vernalisation requirements are fulfilled.
	bool ISVERNALISED; // has the vernalisation been reached?
};

struct WofostCrop {

	WofostCropParameters p;
	WofostCropRates r;
	WofostCropStates s;
		
// variables
	bool alive;
	int emergence, ILVOLD, IDANTH;
	double EFF, AMAX, PGASS, RFTRA, TRANRF;
	double LASUM, KDif, TRAMX;
	double Fr, Fl, Fs, Fo; // FS is a system variable on solaris so renamed to Fs etc
	double TRA=0; 
	double TMINRA, DSLV, SLAT;
	double PMRES;

	std::vector<double> SLA = std::vector<double>(366), LV = std::vector<double>(366), LVAGE = std::vector<double>(366), TMNSAV = std::vector<double>(7);

	
//04/2017 npk
	//double GASST, MREST, CTRAT, HI;

	/*
	WofostCropParametersNPK pn; // nutrient parameters

	struct ratesNPK {
		double RNUSO, RPUSO, RKUSO;
		double RNUPTAKE, RPUPTAKE, RKUPTAKE;
		double RNULV, RNUST, RNURT, RPULV, RPUST, RPURT, RKULV, RKUST, RKURT;
		double RNLV, RNST, RNRT, RNSO, RPLV, RPST, RPRT, RPSO, RKLV, RKST, RKRT, RKSO;
		double RNTLV, RNTST, RNTRT, RNTSO, RPTLV, RPTST, RPTRT, RPTSO, RKTLV, RKTST, RKTRT, RKTSO;
		double RNLOSS, RPLOSS, RKLOSS;
		double RNFIX;

	};
	ratesNPK rn;
	
	struct statesNPK {
		double ANLV, ANST, ANRT, ANSO;
		double APLV, APST, APRT, APSO;
		double AKLV, AKST, AKRT, AKSO;
		double ATNLV, ATNST, ATNRT;
		double ATPLV, ATPST, ATPRT;
		double ATKLV, ATKST, ATKRT;
	};
	statesNPK sn;
	
	struct variablesNPK {
		double NNI, PNI, KNI, NPKI, NPKREF;
		double NTRANSLOCATABLE, PTRANSLOCATABLE, KTRANSLOCATABLE;
		double NDEMLV, NDEMST, NDEMRT, NDEMSO, PDEMLV, PDEMST, PDEMRT, PDEMSO, KDEMLV, KDEMST, KDEMRT, KDEMSO;
		double NUPTAKE_T, KUPTAKE_T, PUPTAKE_T, NFIX_T;
		double NLOSSES_T, KLOSSES_T, PLOSSES_T;

	};
	variablesNPK vn;
	*/

};

/*
struct WofostSoilParametersNPK {
	double BG_N_SUPPLY, BG_P_SUPPLY, BG_K_SUPPLY;
	std::vector<double> N_recovery, P_recovery, K_recovery;
	double NSOILBASE_FR, PSOILBASE_FR, KSOILBASE_FR;
	double NSOILBASE, PSOILBASE, KSOILBASE;
	double NSOILI, PSOILI, KSOILI;
};
*/

struct WofostSoilParameters {

	bool IZT = false; //groundwater present 
	int IFUNRN;
	int NOTINF; // fraction not infiltrating rainfall
	int IDRAIN; // presence of drains
	double SM0, SMFCF, SMW, SOPE, KSUB, CRAIRC, K0, SMLIM, SSI;
	double SSMAX; // max surface storage

	//STDAY
	double SPADS, SPODS, SPASS, SPOSS, DEFLIM;
	
	double WAV, ZTI, DD, RDMSOL;
	
	std::vector<double> SMTAB, CONTAB, PFTAB;
	// currently hard-coded
	std::vector<double> NINFTB;
};


struct WofostSoil {

	WofostSoilParameters p;
	//WofostSoilParametersNPK pn;

// RATES
	double EVS, EVW, CR, DMAX, DZ;
	double RIN, RINold, RIRR, DW, PERC, LOSS, DWLOW;
	
// STATES
	double SM, ss, W, WI, DSLR, WLOW, WLOWI, WWLOW;
// SS is a system variable on SOLARIS. Hence ss.
	
// VARIABLES
	int ILWPER, IDFWOR;
	double RDM, EVWMX, EVSMX;
	double SPAC, SPOC, WEXC, CAPRMX, SEEP, COSUT; 	// STDAY
	double RTDF, MH0, MH1, ZT, SUBAIR, WZ, WZI, WE, WEDTOT, PF;

	// summation  TSR, EVST, EVWT, WDRT, CRT, DRAINT, PERCT, LOSST
	
	std::vector<double> SDEFTB, DEFDTB, CAPRFU;

	/*
	struct ratesNPK {
		double RNSOIL, RPSOIL, RKSOIL;
		double RNAVAIL, RPAVAIL, RKAVAIL;
	};
	ratesNPK rn;

	struct statesNPK {
		double NSOIL, PSOIL, KSOIL;
		double NAVAIL, PAVAIL, KAVAIL;
	};
	statesNPK sn;
	*/

};


struct WofostAtmosphere {
	double RAIN, AVRAD, TEMP, DTEMP, TMIN, TMAX, E0, ES0, ET0, DAYL, DAYLP, WIND, VAP;
	double SINLD, COSLD, DTGA, DSINB, DSINBE, DifPP;
	double ATMTR, ANGOT;
};


struct WofostForcer {
	bool force_DVS=false, force_LAI=false, force_SM=false, force_FR=false, force_DMI=false, force_ADMI=false, force_FL=false, force_PAI=false, force_RFTRA=false, force_SAI=false,
	force_WRT=false, force_WLV=false, force_WSO=false, force_WST=false;
	std::vector<double> DVS, LAI, SM, FR, DMI, ADMI, FL, PAI, RFTRA, SAI, WRT, WLV, WSO, WST;
};


struct WofostOutput {
	std::vector<std::string> names;
	std::vector<double> values;
};



struct WofostModel {

	unsigned step, time, DOY, npk_step;
	int IDHALT, ISTATE;
	//bool IOX;

	std::vector<std::string> messages;
	bool fatalError;

	WofostSoil soil;
	WofostCrop crop;
	WofostControl control;

	WofostAtmosphere atm;
	WofostWeather wth;
	
	WofostForcer forcer;
	void force_states();
	
	WofostOutput output;
	
	bool weather_step();

	void crop_initialize();
	void crop_rates();
	void crop_states();

	void vernalization_initialize();
	void vernalization_states();
	void vernalization_rates();

	//void npk_rates();
	//void npk_states();
/*	void npk_demand_uptake_initialize();
	void npk_demand_uptake_rates();
	void npk_demand_uptake_states();

	void npk_soil_dynamics_initialize();
	void npk_soil_dynamics_states();
	void npk_soil_dynamics_rates();

	void npk_crop_dynamics_initialize();
	void npk_crop_dynamics_rates();
	void npk_crop_dynamics_states();

	void npk_translocation_initialize();
	void npk_translocation_rates();
	void npk_translocation_states();

	void npk_stress();
	void npk_apply();
*/
	//void maintanance_respiration();

	void soil_initialize();
	void soil_rates();
	void soil_states();

	void WATFD_initialize();
	void WATFD_rates();
	void WATFD_states();

	void WATPP_initialize();
	void WATPP_rates();
	void WATPP_states();

	void WATGW_initialize();
	void WATGW_rates();
	void WATGW_states();

	void STDAY_initialize();
	void STDAY();

	void ROOTD_initialize();
	void ROOTD_rates();
	void ROOTD_states();

	void ASTRO();
	void PENMAN();
	void PENMAN_MONTEITH();
	void EVTRA();
	double TOTASS();

	void model_initialize();
	void model_run();
	void model_output();

};


