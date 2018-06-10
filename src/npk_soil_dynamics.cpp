// -*- coding: utf-8 -*-
// Copyright (c) 2004-2015 Alterra, Wageningen-UR
// Allard de Wit and Iwan Supit (allard.dewit@wur.nl), July 2015
// Approach based on LINTUL N/P/K made by Joost Wolf

/*
class NPK_Soil_Dynamics(SimulationObject):
    """A simple module for soil N/P/K dynamics.

    This modules represents the soil as a bucket for available N/P/K consisting
    of two components: 1) a native soil supply which consists of an initial
    amount of N/P/K which will become available with a fixed fraction every day
    and 2) an external supply which is computed as an amount of N/P/K supplied
    and multiplied by a recovery fraction in order to have an effective amount of
    N/P/K that is available for crop growth.

    This module does not simulate any soil physiological processes and is only
    a book-keeping approach for N/P/K availability. On the other hand, it
    requires no detailed soil parameters. Only an initial soil amount, the
    fertilizer inputs, a recovery fraction and a background supply.

    **Simulation parameters**

    ============  ============================================= =======  ==============
     Name          Description                                   Type     Unit
    ============  ============================================= =======  ==============
    NSOILBASE     Base soil supply of N available through        p.SSI      |kg ha-1|
                  mineralisation
    NSOILBASE_FR  Fraction of base soil N that comes available   p.SSI        -
                  every day
    PSOILBASE     Base soil supply of N available through        p.SSI      |kg ha-1|
                  mineralisation
    PSOILBASE_FR  Fraction of base soil N that comes available             -
                  every day
    KSOILBASE     Base soil supply of N available through        p.SSI      |kg ha-1|
                  mineralisation
    KSOILBASE_FR  Fraction of base soil N that comes available   p.SSI        -
                  every day
    DVSNPK_STOP   Development stage after which no nutrients
                  are taken up from the soil by the crop.
    BG_N_SUPPLY   Background supply of N through atmospheric     p.SSI      |kg ha-1 d-1|
                  deposition.
    BG_P_SUPPLY   Background supply of P through atmospheric     p.SSI      |kg ha-1 d-1|
                  deposition.
    BG_K_SUPPLY   Background supply of K through atmospheric     p.SSI      |kg ha-1 d-1|
                  deposition.
    ============  ============================================= =======  ==============


    **State variables**

    =======  ================================================= ==== ============
     Name     Description                                      Pbl      Unit
    =======  ================================================= ==== ============
     NSOIL    total mineral soil N available at start of         N    [kg ha-1]
              growth period
     PSOIL    total mineral soil P available at start of         N    [kg ha-1]
              growth period
     KSOIL    total mineral soil K available at start of         N    [kg ha-1]
              growth period
     NAVAIL   Total mineral N from soil and fertiliser           Y    |kg ha-1|
     PAVAIL   Total mineral N from soil and fertiliser           Y    |kg ha-1|
     KAVAIL   Total mineral N from soil and fertiliser           Y    |kg ha-1|
    =======  ================================================= ==== ============

    **Rate variables**

    ==============  ================================================= ==== =============
     Name            Description                                       Pbl      Unit
    ==============  ================================================= ==== =============
    RNSOIL           Rate of change on total soil mineral N            N   |kg ha-1 d-1|
    RPSOIL           Rate of change on total soil mineral P            N   |kg ha-1 d-1|
    RKSOIL           Rate of change on total soil mineral K            N   |kg ha-1 d-1|

    RNAVAIL          Total change in N availability                    N   |kg ha-1 d-1|
    RPAVAIL          Total change in P availability                    N   |kg ha-1 d-1|
    RKAVAIL          Total change in K availability                    N   |kg ha-1 d-1|

    // Rate of fertilizer supply for N/P/K [kg/ha/day]
    FERT_N_SUPPLY    Supply of fertilizer N. This will be supplied     N   |kg ha-1 d-1|
                     by the AgroManager module through the event
                     mechanism. See the section on signals below.
    FERT_P_SUPPLY    As previous for P                                 N   |kg ha-1 d-1|
    FERT_K_SUPPLY    As previous for K                                 N   |kg ha-1 d-1|
    ==============  ================================================= ==== =============

    **Signals send or handled**

    `NPK_Soil_Dynamics` receives the following signals:
        * APPLY_NPK: Is received when an external input from N/P/K fertilizer
          is provided. See `_on_APPLY_NPK()` for details.

    **External dependencies:**

    =========  =================================== ===================  ==============
     Name       Description                         Provided by          Unit
    =========  =================================== ===================  ==============
    DVS        Crop development stage              DVS_Phenology           -
    TRA        Actual crop transpiration           Evapotranspiration     |cm|
               increase
    TRAMX      Potential crop transpiration        Evapotranspiration     |cm|
               increase
    RNUPTAKE   Rate of N uptake by the crop        NPK_Demand_Uptake     |kg ha-1 d-1|
    RPUPTAKE   Rate of P uptake by the crop        NPK_Demand_Uptake     |kg ha-1 d-1|
    RKUPTAKE   Rate of K uptake by the crop        NPK_Demand_Uptake     |kg ha-1 d-1|
    =========  =================================== ===================  ==============
    """

    NSOILI = Float(-99.) // initial soil N amount
    PSOILI = Float(-99.) // initial soil P amount
    KSOILI = Float(-99.) // initial soil K amount

    class Parameters(ParamTemplate):
        NSOILBASE = Float(-99.)  // total mineral soil N available at start of growth period [kg N/ha]
        NSOILBASE_FR = Float(-99.)  // fraction of soil mineral N coming available per day [day-1]

        PSOILBASE = Float(-99.)  // total mineral soil P available at start of growth period [kg N/ha]
        PSOILBASE_FR = Float(-99.)  // fraction of soil mineral P coming available per day [day-1]

        KSOILBASE = Float(-99.)  // total mineral soil K available at start of growth period [kg N/ha]
        KSOILBASE_FR = Float(-99.)  // fraction of soil mineral K coming available per day [day-1]

        DVSNPK_STOP = Float(-99.)  // Development stage after which no nutrients are absorbed

        // Background rates of N/P/K supply [kg/ha/day]
        BG_N_SUPPLY = Float()
        BG_P_SUPPLY = Float()
        BG_K_SUPPLY = Float()

    class StateVariables(StatesTemplate):
        NSOIL = Float(-99.)  // mineral N available from soil for crop    kg N ha-1
        PSOIL = Float(-99.)  // mineral N available from soil for crop    kg N ha-1
        KSOIL = Float(-99.)  // mineral N available from soil for crop    kg N ha-1

        NAVAIL = Float(-99.)  // total mineral N from soil and fertiliser  kg N ha-1
        PAVAIL = Float(-99.)  // total mineral P from soil and fertiliser  kg N ha-1
        KAVAIL = Float(-99.)  // total mineral K from soil and fertiliser  kg N ha-1

    class RateVariables(RatesTemplate):
        RNSOIL = Float(-99.)
        RPSOIL = Float(-99.)
        RKSOIL = Float(-99.)

        RNAVAIL = Float(-99.)
        RPAVAIL = Float(-99.)
        RKAVAIL = Float(-99.)

        // Rate of fertilizer supply for N/P/K [kg/ha/day]
        FERT_N_SUPPLY = Float()
        FERT_P_SUPPLY = Float()
        FERT_K_SUPPLY = Float()

*/

/*
 def initialize(self, day, kiosk, parvalues):
        // INITIAL STATES
        p = self.params
        self.NSOILI = p.NSOILBASE
        self.PSOILI = p.PSOILBASE
        self.KSOILI = p.KSOILBASE

        self.states = self.StateVariables(kiosk,
            publish=["NAVAIL", "PAVAIL", "KAVAIL"],
            NSOIL=p.NSOILBASE, PSOIL=p.PSOILBASE, KSOIL=p.KSOILBASE,
            NAVAIL=0., PAVAIL=0., KAVAIL=0.)

        self._connect_signal(self._on_APPLY_NPK, signals.apply_npk)
*/


#include <math.h>
#include <vector>
#include "wofost.h"
#include "SimUtil.h"
#include <string.h>
#include <iostream>

void WofostModel::npk_soil_dynamics_initialize() {
  soil_initialize();
  soil.sn.NSOIL = 0;
  soil.sn.PSOIL = 0;
  soil.sn.KSOIL = 0;

  soil.sn.NAVAIL = 0;
  soil.sn.PAVAIL = 0;
  soil.sn.KAVAIL = 0;
}


void WofostModel::npk_soil_dynamics_rates() {

    //04/2017 npk add
    soil_rates();

    double NutrientLIMIT;
    if ((crop.DVS < crop.pn.DVSNPK_STOP) && (crop.TRANRF > 0.01)) {
        NutrientLIMIT = 1.0;
    } else {
        NutrientLIMIT = 0;
		}

    soil.rn.RNSOIL = -max(0., min(soil.pn.NSOILBASE_FR * soil.pn.NSOILBASE * NutrientLIMIT, soil.sn.NSOIL));
    soil.rn.RPSOIL = -max(0., min(soil.pn.PSOILBASE_FR * soil.pn.PSOILBASE * NutrientLIMIT, soil.sn.PSOIL));
    soil.rn.RKSOIL = -max(0., min(soil.pn.KSOILBASE_FR * soil.pn.KSOILBASE * NutrientLIMIT, soil.sn.KSOIL));


	double FERT_N_SUPPLY, FERT_P_SUPPLY, FERT_K_SUPPLY;
	// this should be by date
  //std::cout << "npk_step: " << npk_step << std::endl;
  //std::cout << "simdate: " << wth.date[step-1] << std::endl;
  //std::cout << "npkdates: " << control.NPKdates[npk_step] << std::endl;

  FERT_N_SUPPLY = 0;
  FERT_P_SUPPLY = 0;
  FERT_K_SUPPLY = 0;

  if ( npk_step < control.NPKdates.size() ){
    if ( wth.date[step-1] == control.NPKdates[npk_step] ){
      FERT_N_SUPPLY = control.N_amount[npk_step] * soil.pn.N_recovery[npk_step];
  		FERT_P_SUPPLY = control.P_amount[npk_step] * soil.pn.P_recovery[npk_step];
  		FERT_K_SUPPLY = control.K_amount[npk_step] * soil.pn.K_recovery[npk_step];
      npk_step++;
    }
  }

    //std::cout << "RNUPTAKE: " << crop.rn.RNUPTAKE << std::endl;
    soil.rn.RNAVAIL = FERT_N_SUPPLY + soil.pn.BG_N_SUPPLY - crop.rn.RNUPTAKE - soil.rn.RNSOIL;
    soil.rn.RPAVAIL = FERT_P_SUPPLY + soil.pn.BG_P_SUPPLY - crop.rn.RPUPTAKE - soil.rn.RPSOIL;
    soil.rn.RKAVAIL = FERT_K_SUPPLY + soil.pn.BG_K_SUPPLY - crop.rn.RKUPTAKE - soil.rn.RKSOIL;
}



void WofostModel::npk_soil_dynamics_states() {

    soil_states();
    // mineral NPK amount in the soil
    soil.sn.NSOIL += soil.rn.RNSOIL;
    soil.sn.PSOIL += soil.rn.RPSOIL;
    soil.sn.KSOIL += soil.rn.RKSOIL;

    // total (soil + fertilizer) NPK amount in soil
    soil.sn.NAVAIL += soil.rn.RNAVAIL;
    soil.sn.PAVAIL += soil.rn.RPAVAIL;
    soil.sn.KAVAIL += soil.rn.RKAVAIL;
}
