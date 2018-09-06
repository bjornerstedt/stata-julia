version 13

program julia , rclass
syntax [varlist] , [function(string)] [using(string)] [command(string)]

plugin call calljulia `varlist' `if' `in', "`function'"  "`using'"  "`command'" "`varlist'"

end

* TODO: Load different plugin for each OS
program calljulia, plugin
