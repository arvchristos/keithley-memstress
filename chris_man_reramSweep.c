/* USRLIB MODULE INFORMATION

	MODULE NAME: chris_man_reramSweep
	MODULE RETURN TYPE: int 
	NUMBER OF PARMS: 25
	ARGUMENTS:
		riseTime,	double,	Input,	1e-4,	20e-9,	
		widthTime,	double,	Input,	1e-6,	20e-9,	
		delayTime,	double,	Input,	1e-4,	20e-9,	
		complianceCH,	int,	Input,	2,	1,	2
		resetV,	double,	Input,	-2.5,	-20,	20
		setV,	double,	Input,	2.5,	-20,	20
		Irange,	double,	Input,	1e-2,	0.0,	1
		resetIcomp,	double,	Input,	0.0,	-0.1,	0.1
		setIcomp,	double,	Input,	1e-3,	-0.1,	0.1
		resTestV,	double,	Input,	2,	-10,	10
		takeRmeas,	int,	Input,	1,	0,	1
		useSmu,	int,	Input,	0,	0,	1
		numIter,	double,	Input,	1,	1,	
		Vforce,	D_ARRAY_T,	Output,	,	,	
		Vforce_size,	int,	Input,	1000,	10,	10000
		Imeas,	D_ARRAY_T,	Output,	,	,	
		Imeas_size,	int,	Input,	1000,	10,	10000
		Time,	D_ARRAY_T,	Output,	,	,	
		Time_size,	int,	Input,	1000,	10,	10000
		pts,	int *,	Output,	,	,	
		resetResistance,	double *,	Output,	,	,	
		setResistance,	double *,	Output,	,	,	
		initResistance,	double *,	Output,	,	,	
		loopNum,	int,	Input,	1,	1,	
		stepIncrement,	double,	Input,	0,	-1,	1
	INCLUDES:
#include "keithley.h"
#include "nvm.h"

	END USRLIB MODULE INFORMATION
*/
/* USRLIB MODULE HELP DESCRIPTION
DESCRIPTION:
------------
The chris_man_reramSweep sweep is used to perform a double sweep with a flat section at the peak of each sweep.To test a reram device, choose appropriate values for the two peaks, either positive or negative, and then set the timing you would like to implement.

Note: In the test it is assumed that RPM1 is linked with SMU1 and RPM2 is linked with SMU2.  It is assumed that RPM1 (Channel 1) is connected to the side of the dut with higher capacitance, such as chuck, substrate, which is usually a *lower/bottom side*.  RPM2 (Channel 2) should be connected on the opposite side, which is usually its *top side* to minimize parasitic current transients. Channel 2 forces 0 V and is used to measure current.  If useSmu = 1, SMU1 and SMU2 are used for force voltage and measure current.   If useSmu = 0, pulse force/measurement is performed with PMU.  Voltage bias polarities should be applied, as if bias is applied from the top to simulate standard SMU/DC testing.  Polarities of the forced bias and measured current inverted in the code if you are using the PMU instead of SMU (useSmu = 0).

INPUTS:
-------
riseTime	(double) The time it takes for voltage to ramp to the final value.  This only applies if using PMU (useSmu = 0) if using SMU there is a fixed ramp rate.  Rise time should be longer than 5% of the widthTime.

widthTime	(double) The time to wait at the top of the pulse at full voltage

delayTime	(double) This is the delay time at the before and after each set or reset pulse.  The time between a set and reset pulse will therefore be double this time.. This only applies when the PMU is being used instead of the SMU (useSmu = 0)

complianceCH	(integer) On which SMU channel to enforce current compliance.

resetV		(double) The peak voltage of the reset pulse.  For ReRAM devices, this value should be negative

setV		(double) The peak voltage of the set pulse. For ReRAM devices, this value should be positive

Irange		(double) The range at which to measure current.  

resetIcomp	(double) Current compliance value to enforce during the reset pulse.  

Please note that this variable is used both for PMU and SMU control during RESET.  If it is set to 0, than no current limit is applied.   If useSmu = 1, and sweep is performed with SMU, and current limit is 0, autorange is used.  With current limit not zero and useSmu = 1, SMU is set in the fixed current range.

setIcomp	(double) Current compliance value to enforce during the set pulse.  

Please note that this variable is used both for PMU and SMU control during SET.  If it is set to 0, than no current limit is applied.   If useSmu = 1, and sweep is performed with SMU, and current limit is 0, autorange is used.  With current limit not zero and useSmu = 1, SMU is set in the fixed current range.

Polarities for PMU compliances are selected, based on the a) inversion of the voltage on low side b) polarity of the intended voltage (setV and resetV - are biases of top/PRM2 to the bottom/RPM1), and c) used channel (2 or 1).

resTestV	(double) The voltage at which to measure the resistance of the DUT.  This should be much less than the set and reset voltages as to not set or reset the DUT.

takeRmeas	(integer) Whether or not to take resistance measurements.  1 means yes, take resistance measurements, while 0 means no, do not take resistance measurements.

useSmu		(integer) Whether or not to use a SMU to take DC measurements instead of using the PMU for pulsing measurements.  A 1 means yes, use the SMU and a 0 means no, don't use the SMU, use the PMU.

numIter		(integer) When in pulse mode, number of pulses to perform total.  For instance, a value of 5 would execute 4 pulses without measuring resistance.  Then on the fifth pulse, resistance measurements would be taken.  A value on 1 simply executes the sweep once.

Vforce_size	(Integer)
Imeas_size	(Integer)
Time_size	(Integer) These three values should be the same and represent the number of items in the output arrays.

OUTPUTS:
--------
Vforce		(double) Array of forced voltages
Imeas		(double) Array of measured currents
Time		(double) Array of measured times
resetResistance	(double) Resistance of DUT after the reset pulse
setResistance	(double) Resistance of DUT after the set pulse
initResistance	(double) Resistance of DUT before any pulse

Function returns error status, where:
          1  : success [TEST_SUCCESS]
	-10  : cannot initialize nvm structure, [NVM_INITIALIZE_ERROR]
	-20  : array sizes are not the same [ARRAY_SIZES_DIFFERENT]
	-30  : array size is too small, < 10, [ARRAY_SIZE_ERROR]
	-200  : invalid number of points returned, [NUMBER_OF_POINTS_ERROR]
	-210  : main, pulse_test function failed, [PULSE_TEST_FAILED]
	-240  : used rate is less than minimum allowed, [MIN_RATE_ERROR]
	-250  : dcSweep failed, [DC_SWEEP_FAILED]
	-260   : riseTime is shorter than 5% of the width time [RISE_WIDTH_ERROR]
	END USRLIB MODULE HELP DESCRIPTION */
/* USRLIB MODULE PARAMETER LIST */
#include "keithley.h"
#include "nvm.h"


int chris_man_reramSweep( double riseTime, double widthTime, double delayTime, int complianceCH, double resetV, double setV, double Irange, double resetIcomp, double setIcomp, double resTestV, int takeRmeas, int useSmu, double numIter, double *Vforce, int Vforce_size, double *Imeas, int Imeas_size, double *Time, int Time_size, int *pts, double *resetResistance, double *setResistance, double *initResistance, int loopNum, double stepIncrement )
{
/* USRLIB MODULE CODE */
int stat;
double time_offset=0; // offset to be added to each element of timearr
  //we add desc in comments
  resetV *= -1.0;
  setV *= -1.0;
  int measCH = 2;
double initResetV = resetV;
double initSetV = setV;
for(int p = 0;p<loopNum;p++){
  char mod[] = "chris_man_reramSweep";
  stat = -1;
  int j;
  double resetDVForce[MAX_OUT_PTS];
  double setDVForce[MAX_OUT_PTS];
  double resetIMeas[MAX_OUT_PTS];
  double setIMeas[MAX_OUT_PTS];
  double timeArr[MAX_OUT_PTS];
  double timeArr2[MAX_OUT_PTS];
  double voffset;
  double ioffset;
  double initr;
  double resetr;
  double setr;
  double ttime;
  int numsetpts;
  int numresetpts;
  double toffset, dt;
   double reset_sign, set_sign;
  
  //Setup NVM structures
  NVMS *nvm;
  pmu *pmu1;
  pmuch *ch1, *ch2;
  seg *seg1, *seg2;
  int maxpts = MAX_OUT_PTS, max_rate;


  if(resetV > 0)
      reset_sign = 1.0;
  else 
      reset_sign = -1.0;

  if(setV > 0)
      set_sign = 1.0;
  else 
      set_sign = -1.0;

  //set increment
  resetV = initResetV - (2*p)* stepIncrement;
  setV = initSetV - (2*p+1)* stepIncrement;
  //initialization:
  *resetResistance = 0.0;
  *setResistance = 0.0;
  *initResistance = 0.0;
  
  if(riseTime/widthTime < 0.05)
  {
    nlog("%s: Rise time (%g) is too short relative to the width (%g) !\n", 
        mod, riseTime, widthTime);
    stat = RISE_WIDTH_ERROR;
    goto RETURN;
  }
    
  //Check if output arrays are the same size
  if(Vforce_size != Imeas_size || Vforce_size != Time_size)
    {
      nlog("%s: Make sizes of all output arrays equal!\n", mod);
      stat = ARRAY_SIZES_DIFFERENT;
      goto RETURN;
    }

  //Reset max points according to output array sizes
  //NOTE: max points are only half of the size of the array
  if(maxpts > (Vforce_size/2)) maxpts = Vforce_size/2;

  //make sure miniminum width is 20 ns
  if(widthTime < 2e-8) widthTime = 2e-8;
  
  //Check if output arrays are too small
  if(Vforce_size  < 10)
    {
      nlog("%s: Make sizes of all output arrays at least 20 or more!\n", mod);
      stat = ARRAY_SIZE_ERROR;
      goto RETURN;
    }
  
  //-----------Completely branch off if we are doing more than one loop ---------------------------
  if(numIter > 1) 
    {
        if(useSmu==0)
        {
      
        //Initialize NVM using 1 PMU
        nvm = initNVMST(1);
        nvm->init = 0;
      
        //Check if initialization is successful
        if(NULL == nvm)
        {
          nlog("%s: Cannot initialize NVM structure!\n", mod);
          stat = NVM_INITIALIZE_ERROR; goto RETURN;
        }
        nlog("%s: starts\n", mod);
      
        //Find PMU
        pmu1 = getPMU(1);
      
        //Find Channels 1 and 2
        ch1 = getCH(pmu1,1);//ch1
        ch2 = getCH(pmu1,2);//ch2
      
        //Set measurement mode for both channels
        ch1->mode = PULSE_MEAS_NONE;
        ch2->mode = PULSE_MEAS_NONE;
      
        ch1->irange = Irange;
        ch2->irange = Irange;
        
        ch1->vrange = 10.0;
        ch2->vrange = 10.0;
      
        //Initialize segments
        seg1 = initSEGS(ch1,10);  //ch1 8 segments
        seg2 = initSEGS(ch2,10);  //ch2 8 segments
      
        if(fabs(resetIcomp) > 0)
        {
            if(complianceCH == 1)
                ch1->ilimit = reset_sign * fabs(resetIcomp);
            if(complianceCH == 2)
                ch2->ilimit = reset_sign * (-1.0) * fabs(resetIcomp);
        }
      
        //overwrite by set compliance if not ZERO
        if(fabs(setIcomp) > 0)
        {
            if(complianceCH == 1)
                ch1->ilimit = set_sign * fabs (setIcomp);
            if(complianceCH == 2)
                ch2->ilimit = set_sign * (-1.0) * fabs (setIcomp);
        }
      
        /*~~~~~~Beginning of Setup for Ch1 Segments~~~~~~*/
        seg1[0].segtime = delayTime;

        seg1[1].segtime = riseTime;
        seg1[1].stopv = resetV;
      
        seg1[2].segtime = widthTime;
        seg1[2].stopv = resetV;
      
        seg1[3].segtime = riseTime;
        seg1[4].segtime = delayTime;
        seg1[5].segtime = delayTime;

        seg1[6].segtime = riseTime;
        seg1[6].stopv = setV;
      
        seg1[7].segtime = widthTime;
        seg1[7].stopv = setV;
      
        seg1[8].segtime = riseTime;
        seg1[9].segtime = delayTime;

        /*~~~~~~End of Setup for Ch1 Segments~~~~~~*/
      
        //Set the total time and maximum rate
        ttime = getTotalTime(seg1, 20);
        nvm->total_time = ttime;
      
        //max_rate = calc_rate (ttime, maxpts);
        //pmu1->rate = max_rate;
      
        //Setup Ch2 segment
        copyTime(seg1, seg2, 20);
      
        nlog("%s: total time:%g\n", mod, nvm->total_time); 
        nlog("%s: init: %g\n", mod, nvm->init);
      
        pmu1->loops = numIter-1;
      
        // DO THE TEST
        stat = pulse_test();
        if(0 > stat){stat = PULSE_TEST_FAILED; goto RETURN;};
      
        devint();
    }
    else //do PULSE sweep   //if(useSmu==1)
    {

        j = 0;
        //while(j++ <= numIter -1)//PR47385
        while(j++ <= numIter - 2)
        {    
            //running Reset
            stat = dcSweep("SMU1", "SMU2", 
                 complianceCH, measCH,
                 Irange, resetIcomp, 
                 0.002, widthTime, 
                 (-1.0)*resetV, (Vforce_size/2), 
                 resetDVForce, (Vforce_size/2),
                 resetIMeas, (Vforce_size/2), 
                 timeArr, (Vforce_size/2));

            if(stat < 0)
            {
                nlog("%s: error in dcSweep:%d\n", mod, stat);
                stat = DC_SWEEP_FAILED;
                goto RETURN;
            }

            //runnig set
            stat = dcSweep("SMU1", "SMU2", 
                 complianceCH, measCH,
                 Irange, setIcomp,   
                 0.002, widthTime, 
                 (-1.0)*setV, (Vforce_size/2), 
                 setDVForce, (Vforce_size/2), 
                 setIMeas, (Vforce_size/2), 
                 timeArr2, (Vforce_size/2));
            if(stat < 0)
            {
                nlog("%s: error in dcSweep:%d\n", mod, stat);
                stat = DC_SWEEP_FAILED;
                goto RETURN;
            }

        }

    }
   }
  
  //----------------------------------------MEASURE PART-------------------------------------------
  
  // Get an initial Resistance measurement
  if(takeRmeas == 1)
    {
      stat = getRes2( "SMU2", "SMU1", resTestV, &initr );
      *initResistance = initr;
      PostDataDouble("initResistance", initr);
    }
  
  if(useSmu==0)
    {
      //Initialize NVM using 1 PMU
      nvm = initNVMST(1);
      
      nvm->init = 0;
      
      //Check if initialization is successful
      if(NULL == nvm)
        {
        nlog("%s: Cannot initialize NVM structure!\n", mod);
        stat = NVM_INITIALIZE_ERROR; goto RETURN;
        }
      nlog("%s: starts\n", mod);
      
      //Find PMU
      pmu1 = getPMU(1);
      
      //Find Channels 1 and 2
      ch1 = getCH(pmu1,1);//ch1
      ch2 = getCH(pmu1,2);//ch2
      
      //Set measurement mode for both channels
      ch1->mode = PULSE_MEAS_WFM_PER;
      ch2->mode = PULSE_MEAS_WFM_PER;
      
      ch1->irange = Irange;
      ch2->irange = Irange;
      
      ch1->vrange = 10.0;
      ch2->vrange = 10.0;
            
      //  // DO THE TEST
      
      //Initialize segments
      seg1 = initSEGS(ch1,5); //ch1 5 segments
      seg2 = initSEGS(ch2,5); //ch2 5 segments
      
      if(fabs(resetIcomp) > 0)
      {
        if(complianceCH == 1)
            ch1->ilimit = reset_sign * fabs(resetIcomp);
        if(complianceCH == 2)
            ch2->ilimit = reset_sign * (-1.0) * fabs(resetIcomp);
      }
      
      /*~~~~~~Beginning of Setup for Ch1 Segments~~~~~~*/
      seg1[0].segtime = delayTime;

      seg1[1].segtime = riseTime;
      seg1[1].stopv = resetV;

      seg1[2].segtime = widthTime;
      seg1[2].stopv = resetV;
      
      seg1[3].segtime = riseTime;
      seg1[4].segtime = delayTime;

      /*~~~~~~End of Setup for Ch1 Segments~~~~~~*/

      //Set the total time and maximum rate
      ttime = getTotalTime(seg1, 4);
      nvm->total_time = ttime;
      
      max_rate = calc_rate (ttime, maxpts);
      if(max_rate < NVM_MIN_RATE)
      {
        nlog("%s: calculated rate (%d) is smaller than minimum allowed rate (%d)\n",
            mod, max_rate, NVM_MIN_RATE);
        stat = MIN_RATE_ERROR;
        goto RETURN;
      }
      pmu1->rate = max_rate;
      
      //Setup Ch2 segment
      copyTime(seg1, seg2, 5);
      
      nlog("%s: total time:%g\n", mod, nvm->total_time);
      nlog("%s: init: %g\n", mod, nvm->init);
      
      stat = pulse_test();
      if(0 > stat){stat = PULSE_TEST_FAILED; goto RETURN;};
      
      //Determine the number of points from pulse_test()
      *pts = ch1->out_pts;
      PostDataInt("pts", ch1->out_pts);
      nlog("%s: number of points:%d\n", mod, ch1->out_pts);
      if(*pts < 1)
        {
          nlog("%s: Wrong number of points! %d\n", mod, *pts);
          stat = NUMBER_OF_POINTS_ERROR;
          goto RETURN;
        }
      
      //Get current/voltage offsets
      if(2 == measCH)
        ioffset = getPulseI(ch2, 0);
      else
        ioffset = getPulseI(ch1, 0);
      
      voffset = getPulseV(ch2, 0) - getPulseV(ch1, 0);
      
      nlog("%s: Current Offset is: %g Voltage offset: %g\n", mod, 
           ioffset, voffset);
      
      //Correct data with voltage/current offsets,
      for(j = 0; j < ch1->out_pts; j++)
        {  
            if(2 == measCH)
            {
                ch2->pulseI[j] -= ioffset;
                resetIMeas[j] = ch2->pulseI[j];
            }
            else
            {
                ch1->pulseI[j] -= ioffset;
                resetIMeas[j] = ch1->pulseI[j];
            }
      
            resetDVForce[j] = ch2->pulseV[j] - ch1->pulseV[j] - voffset;
      
            // Get the time
            timeArr[j] = ch1->pulseT[j];
        }
        numresetpts = ch1->out_pts;
      
        devint();
      
    }
    else //do PULSE sweep   //if(useSmu==1)
    {
      // DC option
      stat = dcSweep("SMU1", "SMU2", 
             complianceCH, measCH, 
             Irange, resetIcomp,  
             0.002, widthTime,
             (-1.0)*resetV, (Vforce_size/2), 
             resetDVForce, (Vforce_size/2),
             resetIMeas, (Vforce_size/2), 
             timeArr, (Vforce_size/2));
      if(stat < 0)
        {
          nlog("%s: error in dcSweep:%d\n", mod, stat);
          stat = DC_SWEEP_FAILED;
          goto RETURN;
        }
             
          numresetpts = Vforce_size/2;
        // set up time array
        // toffset = timeArr[Vforce_size/2-2];                        // edit
        // for(j = (Vforce_size/2); j<Vforce_size; j++){
        // timeArr[j] = timeArr2[j-(Vforce_size/2)] + toffset;
        //}
} 
  
  //Determine the resistance of the device
  if(takeRmeas == 1)
    {
      stat = getRes2( "SMU2", "SMU1", resTestV, &resetr );
      *resetResistance = resetr;
      PostDataDouble("resetResistance", resetr);
    }

  
  if(useSmu==0)
    {
      // SET PULSE --------------------------------------------------------
      // force init
      nvm->init = 0;
      
      if(fabs(setIcomp) > 0)
        {
        if(complianceCH == 1)
            ch1->ilimit = set_sign * fabs(setIcomp);
        if(complianceCH == 2)
            ch2->ilimit = set_sign * (-1.0) * fabs(setIcomp);
        }
      
      //set Set Voltages
      seg1[1].stopv = setV;
      seg1[2].stopv = setV;
      
      stat = pulse_test();
      if(0 > stat){stat = PULSE_TEST_FAILED; goto RETURN;};
      
      //Determine the number of points from pulse_test()
      *pts = ch1->out_pts;
      PostDataInt("pts", ch1->out_pts);
      nlog("%s: number of points:%d\n", mod, ch1->out_pts);
      if(*pts < 1)
        {
          nlog("%s: Wrong number of points! %d\n", mod, *pts);
          stat = NUMBER_OF_POINTS_ERROR;
          goto RETURN;
        }
      
      //Get current/voltage offsets
      if(2 == measCH)
            ioffset = getPulseI(ch2, 0);
      else
            ioffset = getPulseI(ch1, 0);
      
      voffset = getPulseV(ch2, 0) - getPulseV(ch1, 0);
      
      nlog("%s: Current Offset is: %g Voltage offset: %g\n", mod, 
           ioffset, voffset);
      
      //Correct data with voltage/current offsets,
      dt = ch1->pulseT[1] - ch1->pulseT[0];//one extra time step
      for(j = 0; j < ch1->out_pts; j++)
        {  
      
            if(2 == measCH)
            {
                ch2->pulseI[j] -= ioffset;
                setIMeas[j] = ch2->pulseI[j];
            }
            else
            {
                ch1->pulseI[j] -= ioffset;
                setIMeas[j] = ch1->pulseI[j];
            }

            //disable voltage offset:
            //voffset = 0.0;
                 
            setDVForce[j] = ch2->pulseV[j] - ch1->pulseV[j] - voffset;
            // Do time
            //timeArr[j + ch1->out_pts] = ch1->pulseT[j] + timeArr[ch1->out_pts-1];
            timeArr[j + ch1->out_pts] = ch1->pulseT[j] + timeArr[ch1->out_pts-1] + dt;
        }
      numsetpts = ch1->out_pts;

      devint();

    }
    else//use_smu = 1
    {
      // DC option
      stat = dcSweep("SMU1", "SMU2", 
             complianceCH, measCH,
             Irange, setIcomp, 
             0.002, widthTime, 
             (-1.0)*setV, (Vforce_size/2), 
             setDVForce, (Vforce_size/2), 
             setIMeas, (Vforce_size/2), 
             timeArr2, (Vforce_size/2));

      if(stat < 0)
        {
          nlog("%s: error in dcSweep:%d\n", mod, stat);
          stat = DC_SWEEP_FAILED;
          goto RETURN;
        }
      
      numsetpts = Vforce_size/2;
      // set up the time array
      toffset = timeArr[Vforce_size/2-2];

      for(j = (Vforce_size/2); j<Vforce_size; j++){
      timeArr[j] = timeArr2[j-(Vforce_size/2)] + toffset ;
      }

    }        
  
  if(takeRmeas == 1)
    {
      stat = getRes2( "SMU2", "SMU1", resTestV, &setr );
      *setResistance = setr;
      PostDataDouble("resetResistance", setr);
    }


  //Print data to the appropriate buffers
  PostDataDoubleBuffer("Vforce", resetDVForce, numresetpts);
  PostDataDoubleBuffer("Imeas", resetIMeas, numresetpts);
  // We add time_offset to each element of array before printing.This ensues linear time results.
  for(int k=0; k<numresetpts*2; ++k){
    timeArr[k] += time_offset;     
  }
  time_offset= timeArr[numresetpts*2-1] + timeArr[numresetpts*2-1] - timeArr[numresetpts*2-2];
  


  PostDataDoubleBuffer("Time", timeArr, (numresetpts*2));

  PostDataDoubleBuffer("Vforce", setDVForce, numresetpts);
  PostDataDoubleBuffer("Imeas", setIMeas, numresetpts);


  //Sweep Completed Successfully
  stat = TEST_SUCCESS;

 RETURN:
  //printNVMST();
  nlog("%s: exiting with status:%d\n", mod, stat);
}

  return stat;
/* USRLIB MODULE END  */
} 		/* End chris_man_reramSweep.c */

