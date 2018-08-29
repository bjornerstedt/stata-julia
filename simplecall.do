capture program drop simplecall
program simplecall, plugin

* Note that a plugin cannot create a new Stata variable
scalar x = 0

* Note that a plugin is called with the plugin command,
* not just with its name.
plugin call simplecall , "sqrt(2)"
display x
