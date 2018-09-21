version 13

program julia , rclass
syntax [varlist] [if] [in], [function(name)] [module(name)] [command(string)] [setvariables(namelist)] ///
[matrices(namelist)] [setmatrices(namelist)] [scalars(namelist)] [setscalars(namelist)] ///
[macros(namelist)] [setmacros(namelist)] [save(string)]


plugin call statajulia `varlist' `if' `in' , "`function'"  "`module'"  "`command'"  "`save'" ///
 "`varlist'" "`setvariables'" "`matrices'" "`setmatrices'" "`scalars'" "`setscalars'" ///
 "`macros'" "`setmacros'"

 *program drop calljulia

end

* TODO: Load different plugin for each OS
program statajulia, plugin
