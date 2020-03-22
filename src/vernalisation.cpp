/*
C++ code by Robert Hijmans
Based on Python code from PCSE by Allard de Wit

Modification of phenological development due to vernalisation.
    
The vernalization approach here is based on the work of Lenny van Bussel (2011), which in turn is based on Wang and Engel (1998). The basic principle is that winter wheat needs a certain number of days with temperatures within an optimum temperature range to complete its vernalisation     requirement. Until the vernalisation requirement is fulfilled, the crop development is delayed.
    
The rate of vernalization (VERNR) is defined by the temperature response function VERNRTB. Within the optimal temperature range 1 day is added to the vernalisation state (VERN). The reduction on the phenological development is calculated from the base and saturated vernalisation requirements (VERNBASE and VERNSAT). The reduction factor (VERNFAC) is scaled linearly between VERNBASE and VERNSAT.
    
A critical development stage (VERNDVS) is used to stop the effect of vernalisation when this DVS is reached. This is done to improve model stability in order to avoid that Anthesis is never reached due to a somewhat too high VERNSAT. A warning is given if this happens.   
    
* Van Bussel, 2011. From field to globe: Upscaling of crop growth modelling. Wageningen PhD thesis. http://edepot.wur.nl/180295
* Wang and Engel, 1998. Simulation of phenological development of wheat crops. Agric. Systems 58:1 pp 1-24

*/


#include "wofost.h"

// Helper variable to indicate that DVS > VERNDVS
//    _force_vernalisation = Bool(False)


void WofostModel::vernalization_initialize() {
	crop.s.ISVERNALISED = true; 
	if (crop.p.IDSL > 1) {
		crop.s.ISVERNALISED = false; 	
	}
	crop.r.VERNR = 0;    // Rate of vernalisation
	crop.r.VERNFAC = 1;  // Red. factor for phenol. devel.
	crop.s.VERN = 0;     // Vernalisation state
	crop.s.DOV = 0;      // Day when vernalisationrequirements are fulfilled
}


/* 
Rate variables
=======  ================================================= ==== ============
 Name     Description                                      Pbl      Unit
=======  ================================================= ==== ============
VERNR    Rate of vernalisation                              N     -
VERNFAC  Reduction factor on development rate due to        Y     -
         vernalisation effect.
=======  ================================================= ==== ============
*/


void WofostModel::vernalization_rates() {
    if (! crop.s.ISVERNALISED) {
        if (crop.s.DVS < crop.p.VERNDVS) {
            crop.r.VERNR = AFGEN(crop.p.VERNRTB, atm.TEMP);
            double rr = (crop.s.VERN - crop.p.VERNBASE)/(crop.p.VERNSAT - crop.p.VERNBASE);
            crop.r.VERNFAC = LIMIT(0., 1., rr);
        } else {
			crop.r.VERNR = (crop.p.VERNSAT - crop.s.VERN) + 1e-08;
			std::string msg = "Vernalization forced (VERNDVS reached)";
			messages.push_back(msg);
		}
	} else {
        crop.r.VERNR = 0;
        crop.r.VERNFAC = 1;
	}
}


/* 
State variables
============ ================================================= ==== ========
 Name        Description                                       Pbl   Unit
============ ================================================= ==== ========
VERN         Vernalisation state                                N    days
DOV          Day when vernalisation requirements are            N    -
             fulfilled.
ISVERNALISED Flag indicated that vernalisation                  Y    -
             requirement has been reached
============ ================================================= ==== ========
*/

void WofostModel::vernalization_states() {
	if (!crop.s.ISVERNALISED) {
		crop.s.VERN += crop.r.VERNR;    
		if (crop.s.VERN >= crop.p.VERNSAT) {  
			crop.s.ISVERNALISED = true;
            crop.s.DOV = time;
		} 
	}	
}

