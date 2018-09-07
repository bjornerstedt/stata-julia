version 13

program julia , rclass
syntax [varlist] , [function(string)] [using(string)] [command(string)] [setvariables(namelist)] ///
[matrices(namelist)] [setmatrices(namelist)] [scalars(namelist)] [setscalars(namelist)] ///
[macros(namelist)] [setmacros(namelist)]

plugin call calljulia `varlist' `if' `in', "`function'"  "`using'"  "`command'" ///
 "`varlist'" "`setvariables'" "`matrices'" "`setmatrices'" "`scalars'" "`setscalars'" "`macros'" "`setmacros'"

end

* TODO: Load different plugin for each OS
program calljulia, plugin
