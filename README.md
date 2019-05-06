

Introduction:
chris_man_memstress.c is a module designed to implement ISPP(Incremental Step Pulse Programming) and ECC(Error Checking/Correction) algorithm in Flash memories. 


Requirements:

Keithley 4200-Semiconductor Characterization System.
*This module has been tested succesfully on this system, however it can be ported to other systems.*
NVM Keithley library.


Installation:

Due to copyright issues, chris_man_memstress.c should be installed with the following steps:

1. Through KULT create a new module in nvm library(or a copy of it).
2. Copy KULT_body.c contents to the created module.
3. Manually add chris_man_memstress() parameters in KULT interface.
4. Save and compile module.
5. Build library.
Optional:
6. In order to use KITE's GUI, edit the corresponding xml file adding module's preferences(e.g. GUI_xml_module.xml).


