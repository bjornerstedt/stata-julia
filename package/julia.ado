version 13

program julia , rclass
syntax [varlist] [if] [in], [FUNCtion(name)] [module(name)] [command(string)] [SETVARiables(namelist)] ///
[MATrices(namelist)] [SETMATrices(namelist)] [SCALars(namelist)] [SETSCALars(namelist)] ///
[MACros(namelist)] [SETMACros(namelist)] [save(string)]


plugin call statajulia `varlist' `if' `in' , "`function'"  "`module'"  "`command'"  "`save'" ///
 "`varlist'" "`setvariables'" "`matrices'" "`setmatrices'" "`scalars'" "`setscalars'" ///
 "`macros'" "`setmacros'"

 *program drop calljulia

end

* TODO: Load different plugin for each OS
program statajulia, plugin
