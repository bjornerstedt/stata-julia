version 13

program julia
syntax [varlist] [if] [in] , command(string)

plugin call calljulia `varlist' `if' `in', `command'

end

* Load different plugin for each OS
program calljulia, plugin
