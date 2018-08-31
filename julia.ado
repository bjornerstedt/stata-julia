version 13

program julia
syntax [varlist] [if] [in] , method(string) using(string) [command(string)]

plugin call calljulia `varlist' `if' `in', "`method'"  "`using'"  "`command'" 

end

* Load different plugin for each OS
program calljulia, plugin
