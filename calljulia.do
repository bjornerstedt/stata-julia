capture program drop julia

* Note that a plugin cannot create a new Stata variable
scalar x = 0

julia , command(sqrt(2))
* Note that a plugin is called with the plugin command,
* not just with its name.
display x
