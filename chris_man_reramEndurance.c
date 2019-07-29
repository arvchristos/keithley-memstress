/* USRLIB MODULE INFORMATION

	MODULE NAME: chris_man_reramEndurance
	MODULE RETURN TYPE: int 
	NUMBER OF PARMS: 19
	ARGUMENTS:
		riseTime,	double,	Input,	1e-4,	20e-9,	10e-3
		widthTime,	double,	Input,	1e-6,	20e-9,	10e-3
		delayTime,	double,	Input,	1e-4,	20e-9,	10e-3
		useSmu,	int,	Input,	0,	0,	1
		complianceCH,	int,	Input,	2,	1,	2
		resetV,	double,	Input,	-2.5,	-20,	20
		setV,	double,	Input,	2.5,	-20,	20
		Irange,	double,	Input,	1e-2,	0.0,	0.2
		resetIcomp,	double,	Input,	0.0,	-0.1,	0.1
		setIcomp,	double,	Input,	1e-3,	-0.1,	0.1
		resTestV,	double,	Input,	.5,	-10,	10
		max_loops,	double,	Input,	500,	1,	1e12
		resetRes,	double *,	Output,	,	,	
		setRes,	double *,	Output,	,	,	
		initRes,	double *,	Output,	,	,	
		Vforce,	double *,	Output,	,	,	
		debug_out,	char *,	Output,	"OK",	,	
		debug_double,	double *,	Output,	0,	,	
		Time,	double *,	Output,	,	,	
	INCLUDES:
#include "keithley.h"
#include "nvm.h"
	END USRLIB MODULE INFORMATION
*/
/* USRLIB MODULE HELP DESCRIPTION
DESCRIPTION:
------------
The chris_man_reramEndurance routine is used to perform a series of double sweeps using the same parameters used for the single sweeps as described in the reramSweep routine.

To test a reram device, choose appropriate values for the two peaks, either positive or negative, and then set the timing you would like to implement.  Choose the number of times you would like to run the double sweep and how often you want to take measurements.

Note: In the test it is assumed that RPM1 is linked with SMU1 and RPM2 is linked with SMU2.  It is assumed that RPM1 (Channel 1) is connected to the side of the dut with higher capacitance, such as chuck, substrate, which is usually a *lower/bottom side*.  RPM2 (Channel 2) should be connected on the opposite side, which is usually its *top side* to minimize parasitic current transients. Channel 2 forces 0 V and is used to measure current.  If useSmu = 1, SMU1 and SMU2 are used for force voltage and measure current.   If useSmu = 0, pulse force/measurement is performed with PMU.  Voltage bias polarities should be applied, as if bias is applied from the top to simulate standard SMU/DC testing.  Polarities of the forced bias and measured current inverted in the code if you are using the PMU instead of SMU (useSmu = 0).

INPUTS:
-------
riseTime	(double) The time it takes for voltage to ramp to the final value.  This only applies if using PMU (useSmu = 0) if using SMU there is a fixed ramp rate. Rise time should be longer than 5% of the widthTime.

widthTime	(double) The time to wait at the top of the pulse at full voltage

delayTime	(double) This is the delay time at the before and after each set or reset pulse.  The time between a set and reset pulse will therefore be double this time.. This only applies when the PMU is being used instead of the SMU (useSmu = 0)

useSmu		(integer) Should stay zero.  Used for debugging purposes

complianceCH	(integer) On which SMU channel to enforce current compliance

resetV		(double) The peak voltage of the reset pulse.  For ReRAM devices, this value should be negative

setV		(double) The peak voltage of the set pulse. For ReRAM devices, this value should be positive

Irange		(double) The range at which to measure current.  

resetIcomp	(double) Current compliance value to enforce during the reset pulse.  This value should be entered positive.   

Please note that this variable is used both for PMU and SMU control during RESET.  If it is set to 0, than no current limit is applied.   If useSmu = 1, and sweep is performed with SMU, and current limit is 0, autorange is used.  With current limit not zero and useSmu = 1, SMU is set in the fixed current range.

setIcomp	(double) Current compliance value to enforce during the set pulse.  This value should be positive

Please note that this variable is used both for PMU and SMU control during SET.  If it is set to 0, than no current limit is applied.   If useSmu = 1, and sweep is performed with SMU, and current limit is 0, autorange is used.  With current limit not zero and useSmu = 1, SMU is set in the fixed current range.

resTestV	(double) The voltage at which to measure the resistance of the DUT.  This should be much less than the set and reset voltages as to not set or reset the DUT.

max_loops	(double) Number of pulses to perform total.  This includes both the measure and non-measure pulses.

fatigue_count	(double) Number of times to measure during the max_loops tests.

Note: fatigue_count, max_loops are DOUBLE, not INTEGER - to allow 1e12 number of cycles.

OUTPUTS:
--------
resetResistance	(double) Resistance of DUT after the reset pulse
setResistance	(double) Resistance of DUT after the set pulse
initResistance	(double) Resistance of DUT before any pulse

To view the results, plot iteration on the X axis and plot setResistance and resetResistance on the same Y axis

Function returns error status, where:
      1   : success [TEST_SUCCESS]
	-10   : cannot initialize nvm structure, [NVM_INITIALIZE_ERROR]
	-20   : array sizes are not the same [ARRAY_SIZES_DIFFERENT]
	-30   : array size is too small, < 10, [ARRAY_SIZE_ERROR]
	-200  : invalid number of points returned, [NUMBER_OF_POINTS_ERROR]
	-210  : main, pulse_test function failed, [PULSE_TEST_FAILED]
	-240  : used rate is less than minimum allowed, [MIN_RATE_ERROR]
	-250  : dcSweep failed, [DC_SWEEP_FAILED]
	-260  : riseTime is shorter than 5% of the width time [RISE_WIDTH_ERROR]

	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "keithley.h"
#include "nvm.h"

int chris_man_reramEndurance( double riseTime, double widthTime, double delayTime, int useSmu, int complianceCH, double resetV, double setV, double Irange, double resetIcomp, double setIcomp, double resTestV, double max_loops, double *resetRes, double *setRes, double *initRes, double *Vforce, char *debug_out, double *debug_double, double *Time )
{
/* USRLIB MODULE CODE */
  char mod[] = "chris_man_reramEndurance";
  int stat = -1, j;
  double num_Iter;
  double total = 0;
  int takeRmeas = 1;//do DC measurements
  int pts = 40;
  double Imeas[1000];
  int pts_returned;
  double resetR, setR, initR;


   if(riseTime/widthTime < 0.05)
  {
    nlog("%s: Rise time (%g) is too short relative to the width (%g) !\n", 
        mod, riseTime, widthTime);
    stat = RISE_WIDTH_ERROR;
    goto RETURN;
  }
    //double Time[1000];    
  for(j=0; j< 30; j++)
    {
      //double Vforce[1000];

     // num_Iter = find_iter(max_loops, fatigue_count, j, &total);
      //nlog("%s: timer:%d\n", mod, num_Iter);
    num_Iter = 1;
      stat = reramSweep( riseTime, widthTime, delayTime,
             complianceCH, 
             resetV, 
             setV, Irange, 
             resetIcomp, setIcomp, 
             resTestV, takeRmeas, 
             useSmu, num_Iter, 
             Vforce, pts,
             Imeas, pts,
             Time, pts,
             &pts_returned,
             &resetR, 
             &setR, 
             &initR );

      if(stat < 0)
        goto RETURN;

      PostDataDouble("resetRes", resetR);
      PostDataDouble("setRes", setR);
      PostDataDouble("initRes", initR);
      PostDataDouble("iteration", total);
    //PostDataDoubleBuffer("Vforceout", Vforce,pts_returned*2);
    //PostDataDoubleBuffer("Timeout", Time1, (pts_returned));
    //PostDataDoubleBuffer("Timeout", Time1, (pts_returned*2));
    PostDataString("debug_out","finished");
    PostDataDouble("debug_double", pts_returned);

    }
  stat = 1;

 RETURN:
  nlog("%s: exiting with status:%d\n", mod, stat);
  return stat;

/* USRLIB MODULE END  */
} 		/* End chris_man_reramEndurance.c */

