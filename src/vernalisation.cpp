/*
Author Allard de Wit
Python code from PCSE --- to be translated and integrated.

Modification of phenological development due to vernalisation.
    
The vernalization approach here is based on the work of Lenny van Bussel (2011), which in turn is based on Wang and Engel (1998). The basic principle is that winter wheat needs a certain number of days with temperatures within an optimum temperature range to complete its vernalisation     requirement. Until the vernalisation requirement is fulfilled, the crop development is delayed.
    
The rate of vernalization (VERNR) is defined by the temperature response function VERNRTB. Within the optimal temperature range 1 day is added to the vernalisation state (VERN). The reduction on the phenological development is calculated from the base and saturated vernalisation requirements (VERNBASE and VERNSAT). The reduction factor (VERNFAC) is scaled linearly between VERNBASE and VERNSAT.
    
A critical development stage (VERNDVS) is used to stop the effect of vernalisation when this DVS is reached. This is done to improve model stability in order to avoid that Anthesis is never reached due to a somewhat too high VERNSAT. A warning is given if this happens.   
    
* Van Bussel, 2011. From field to globe: Upscaling of crop growth modelling. Wageningen PhD thesis. http://edepot.wur.nl/180295
* Wang and Engel, 1998. Simulation of phenological development of wheat crops. Agric. Systems 58:1 pp 1-24

*Simulation parameters* (provide in cropdata dictionary)
    
======== ============================================= =======  ============
Name     Description                                   Type     Unit
======== ============================================= =======  ============
VERNSAT  Saturated vernalisation requirements           SCr        days
VERNBASE Base vernalisation requirements                SCr        days
VERNRTB  Rate of vernalisation as a function of daily   TCr        -
         mean temperature.
VERNDVS  Critical development stage after which the     SCr        -
         effect of vernalisation is halted
======== ============================================= =======  ============

**State variables**

============ ================================================= ==== ========
 Name        Description                                       Pbl   Unit
============ ================================================= ==== ========
VERN         Vernalisation state                                N    days
DOV          Day when vernalisation requirements are            N    -
             fulfilled.
ISVERNALISED Flag indicated that vernalisation                  Y    -
             requirement has been reached
============ ================================================= ==== ========


**Rate variables**

=======  ================================================= ==== ============
 Name     Description                                      Pbl      Unit
=======  ================================================= ==== ============
VERNR    Rate of vernalisation                              N     -
VERNFAC  Reduction factor on development rate due to        Y     -
         vernalisation effect.
=======  ================================================= ==== ============


**External dependencies:**

============ =============================== ========================== =====
 Name        Description                         Provided by             Unit
============ =============================== ========================== =====
DVS          Development Stage                 Phenology                 -
             Used only to determine if the
             critical development stage for
             vernalisation (VERNDVS) is
             reached.
============ =============================== ========================== =====
*/

/*

// Helper variable to indicate that DVS > VERNDVS
    _force_vernalisation = Bool(False)

class Parameters(ParamTemplate):
    VERNSAT = Float(-99.)     # Saturated vernalisation requirements
    VERNBASE = Float(-99.)     # Base vernalisation requirements
    VERNRTB = AfgenTrait()    # Vernalisation temperature response
    VERNDVS = Float(-99.)     # Critical DVS for vernalisation fulfillment

class RateVariables(RatesTemplate):
    VERNR = Float(-99.)        # Rate of vernalisation
    VERNFAC = Float(-99.)      # Red. factor for phenol. devel.

class StateVariables(StatesTemplate):
    VERN = Float(-99.)              # Vernalisation state
    DOV = Instance(datetime.date)  # Day when vernalisation
                                        # requirements are fulfilled
    ISVERNALISED =  Bool()              # True when VERNSAT is reached and
                                        # Forced when DVS > VERNDVS

//------------------------------------------------------------------------
def initialize(self, day, kiosk, parvalues):
    """
    :param day: start date of the simulation
    :param kiosk: variable kiosk of this PCSE  instance
    :param cropdata: dictionary with WOFOST cropdata key/value pairs

    """
    self.params = self.Parameters(parvalues)
    self.rates = self.RateVariables(kiosk, publish=["VERNFAC"])
    self.kiosk = kiosk

    # Define initial states
    self.states = self.StateVariables(kiosk, VERN=0., VERNFAC=0.,
                                      DOV=None, ISVERNALISED=False,
                                      publish=["ISVERNALISED"])
//---------------------------------------------------------------------------
//prepare_rates
def calc_rates(self, day, drv):
    rates = self.rates
    states = self.states
    params = self.params

    DVS = self.kiosk["DVS"]
    if not states.ISVERNALISED:
        if DVS < params.VERNDVS:
            rates.VERNR = params.VERNRTB(drv.TEMP)
            r = (states.VERN - params.VERNBASE)/(params.VERNSAT-params.VERNBASE)
            rates.VERNFAC = limit(0., 1., r)
        else:
            rates.VERNR = 0.
            rates.VERNFAC = 1.0
            self._force_vernalisation = True
    else:
        rates.VERNR = 0.
        rates.VERNFAC = 1.0

//---------------------------------------------------------------------
//prepare_states
def integrate(self, day, delt=1.0):
    states = self.states
    rates = self.rates
    params = self.params
    
    states.VERN += rates.VERNR
    
    if states.VERN >= params.VERNSAT:  # Vernalisation requirements reached
        states.ISVERNALISED = True
        if states.DOV is None:
            states.DOV = day
            msg = "Vernalization requirements reached at day %s."
            self.logger.info(msg % day)

    elif self._force_vernalisation:  # Critical DVS for vernalisation reached
        # Force vernalisation, but do not set DOV
        states.ISVERNALISED = True

        # Write log message to warn about forced vernalisation
        msg = ("Critical DVS for vernalization (VERNDVS) reached " +
               "at day %s, " +
               "but vernalization requirements not yet fulfilled. " +
               "Forcing vernalization now (VERN=%f).")
        self.logger.warning(msg % (day, states.VERN))

    else:  # Reduction factor for phenologic development
        states.ISVERNALISED = False
*/
