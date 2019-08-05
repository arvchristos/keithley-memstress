# Keithley ReRAM scripts

## Introduction
This repository includes a number of ReRAM characterization and stress testing scripts compatible with Keithley semiconductor characterization system. Implementaions were co-developed in favor of [Electronic Nanomaterials and Devices Group](http://www.physics.ntua.gr/~tsoukalas/index.html) of National Technical University of Athens (NTUA) by:

* Arvanitis Christos ([@arvchristos](https://github.com/arvchristos))
* Manos Bagakis ([@manosbagakis](https://github.com/manosbagakis))
  
### MemStress

chris_man_memstress.c is a module designed to implement ISPP(Incremental Step Pulse Programming) and ECC(Error Checking/Correction) algorithm in Flash memories. More info about this algorithm can be found [here](https://ieeexplore.ieee.org/document/7942019/citations?tabFilter=papers#citations) 

### ReRAM endurance

chris_man_reramEndurance.c is a module variation in order to achieve pulse test automation. This module enables the user to make repeated pulse tests in Flash Memories.

### ReRAM sweep

chris_man_reramSweep.c is a module designed to implement multiple pulse tests with varying voltage input. The user decides the initial voltage input, the voltage increment per test repetition, as well as the total number of pulses given. This module is used for calculating resistances in Flash Memories under various voltages.

### Retention

chris_man_retention.c is a module designed to execute pulse test for an extended amount of time. Pulse tests are being executed untill given time is elasped.
This module is used for monitoring the resistance of a Flash Memory as time elaspes. Note that this module is designed to be executed for long time periods (hours or even days).

## Requirements

Keithley 4200-Semiconductor Characterization System.
*These modules have been tested succesfully on this system, however they can be ported to other systems.*
NVM Keithley library

## Installation

Due to copyright issues, chris_man_memstress.c should be installed with the following steps:

1. Through KULT create a new module in nvm library(or a copy of it).
2. Copy KULT_body.c contents to the created module.
3. Manually add chris_man_memstress() parameters in KULT interface.
4. Save and compile module.
5. Build library.
Optional:
6. In order to use KITE's GUI, edit the corresponding xml file adding module's preferences(e.g. GUI_xml_module.xml).

*In order to use the other modules, the same approach is recommended. KULT_body code consists of the main block code ,omitting however the declaration of main function as well its parameters.*
