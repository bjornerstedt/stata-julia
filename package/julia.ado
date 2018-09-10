version 13

program julia , rclass
syntax [varlist] [if] [in], [function(string)] [using(string)] [command(string)] [setvariables(namelist)] ///
[matrices(namelist)] [setmatrices(namelist)] [scalars(namelist)] [setscalars(namelist)] ///
[macros(namelist)] [setmacros(namelist)] [save(string)]

plugin call calljulia `varlist' `if' `in' , "`function'"  "`using'"  "`command'" ///
 "`varlist'" "`setvariables'" "`matrices'" "`setmatrices'" "`scalars'" "`setscalars'" ///
 "`macros'" "`setmacros'" "`if'`in'" "`save'"

end

* TODO: Load different plugin for each OS
program calljulia, plugin
