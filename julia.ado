version 13

program julia , rclass
syntax [varlist] [if] [in] , [method(string)] [using(string)] [command(string)]

plugin call calljulia `varlist' `if' `in', "`method'"  "`using'"  "`command'"

end

* TODO: Load different plugin for each OS
program calljulia, plugin
