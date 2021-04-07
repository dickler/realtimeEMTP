############################################################################

This folder contains all files to recreate the original Vivado VHDL project.
Note it's necessary at least a 2020.2 Vivado version.

To recreate the VHDL project, follow the steps below:

1. Open the Vivado TCL Shell for Windows;

2. With the `cd` command, open the directory path of this files;
	
	`cd directory_path`

3. Use the command `source` to run the "realtime_2.tcl" script;
	
	`source realtime_2.tcl`

4. The project is recreated after script is runned.

############################################################################

This especific project is a boost converter implementation with a controller
for the switch. So, there is a design source named "s_switch" that refers to
this controller. For other topologies only disable this design and change
architeture for receive switch commands from IO pins in VC707.
