
/* USRLIB MODULE CODE */
/* USRLIB MODULE CODE */
int stat;
  //we add desc in comments
  Vin *= -1.0;
  resTestV *= -1.0;
  int measCH = 2;
  char mod[] = "chris_man_memstress";
  double voffset;
  double ioffset;
  //double resetr;
  double ttime;
  
  //Setup NVM structures
  NVMS *nvm;
  pmu *pmu1;
  pmuch *ch1, *ch2;
  seg *seg1, *seg2;
  int maxpts = MAX_OUT_PTS, max_rate;
  
//internal variables
  double V_sign; //set_sign;
  int prionia_counter; //counter for number of pulses (steps) in each prioni

  int prionia_limit = 100; //limit for number of pulses (steps) in each prioni

//input_variables
 // double zone_high = zone_low + zone_range; //upper bound of input zone ranged from zone low to high 
//initV // field of gui that is the starting Voltage on each prioni
  double V_after_step = Vin; 

  double prev_resistance ;

    //output
    double V_after_step_resistance;
for(int iter=0; iter<loop_num; iter++){    
prionia_counter = 0;
while(prionia_counter < prionia_limit){
  stat = -1;

  if(Vin > 0)
      V_sign = 1.0;
  else 
      V_sign = -1.0;

  //initialization:
  *resetResistance = 0.0;

  //calculate voltage to send for measurements afters step increment
  V_after_step = Vin - prionia_counter* stepIncrement;

  if(riseTime/widthTime < 0.05)
  {
    nlog("%s: Rise time (%g) is too short relative to the width (%g) !\n", 
        mod, riseTime, widthTime);
    stat = RISE_WIDTH_ERROR;
    goto RETURN;
  }
    
    maxpts = 250;
  //make sure miniminum width is 20 ns
  if(widthTime < 2e-8) widthTime = 2e-8;
  
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
            ch1->ilimit = V_sign * fabs(resetIcomp);
        if(complianceCH == 2)
            ch2->ilimit = V_sign * (-1.0) * fabs(resetIcomp);
      }
      
      /*~~~~~~Beginning of Setup for Ch1 Segments~~~~~~*/
      seg1[0].segtime = delayTime;

      seg1[1].segtime = riseTime;
    
      seg1[1].stopv = V_after_step;

      seg1[2].segtime = widthTime;
      seg1[2].stopv = V_after_step;
      
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
    
        devint();
     
  //Determine the resistance of the device
  if(takeRmeas == 1) //delete in final
    {
      stat = getRes2( "SMU2", "SMU1", resTestV, &V_after_step_resistance );
      *resetResistance = V_after_step_resistance; //not sure if needed !!
      PostDataDouble("resetResistance", V_after_step_resistance);
      PostDataDouble("Vforce", (-1)*V_after_step); //post the corresponding Voltage for this prioni step
    }

       //increment counter for each prioni count
       prionia_counter++;

       //hold the last resistance measurement
       prev_resistance = V_after_step_resistance;


       //check in what range is the previous measurement
       if (V_after_step_resistance > zone_low) //above zone 
       {
           /*TO BE FUNCTIONED */

                              stat = -1;

                              //initialization:
                              *resetResistance = 0.0;


                              if(riseTime/widthTime < 0.05)
                              {
                                nlog("%s: Rise time (%g) is too short relative to the width (%g) !\n", 
                                    mod, riseTime, widthTime);
                                stat = RISE_WIDTH_ERROR;
                                goto RETURN;
                              }
                                
                                maxpts = 250;
                              //make sure miniminum width is 20 ns
                              if(widthTime < 2e-8) widthTime = 2e-8;
                              
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
                                        ch1->ilimit = -1 * fabs(resetIcomp);
                                    if(complianceCH == 2)
                                        ch2->ilimit = -1 * (-1.0) * fabs(resetIcomp);
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
                                
                                    devint();
                                 
                              //Determine the resistance of the device
                              if(takeRmeas == 1) //delete in final
                                {
                                  stat = getRes2( "SMU2", "SMU1", resTestV, &V_after_step_resistance );
                                  *resetResistance = V_after_step_resistance; //not sure if needed !!
                                  PostDataDouble("resetResistance", V_after_step_resistance);
                                  PostDataDouble("Vforce", resetV); //post the resetv Voltage for this prioni step above range
                                    PostDataInt("Verifying_pulses", prionia_counter); //post the number of pulses sent

                                    if (prev_resistance < zone_high)
                                  {
                                    PostDataDouble("Resistance_diff", V_after_step_resistance - prev_resistance); //post the difference for resistances
                                  }
                                  else{
                                    PostDataDouble("Resistance_diff", -1.0); //-1.0 equivalent to null
                                  }
                                  break; // go to next prioni 

                                }
            /*TO BE FUNCTIONED */
       }
       else{
        //do nothing mothafacka
       }
      //Sweep Completed Successfully
      stat = TEST_SUCCESS;
      nlog("%s: exiting with status:%d\n", mod, stat);


 }
}
 RETURN:
  //printNVMST();
  return stat;
/* USRLIB MODULE END  */