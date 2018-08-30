clear all
capture program drop hello
program hello, plugin

* Example dataset:
set obs 5
gen n = _n
gen v = sin(n)
gen nv = 0.0

scalar x = 2
local macro Testing!!
global macro Global!!

matrix A = (1.1,2.2\3.2,4.4)
matrix B = A
* Note that a plugin is called with the plugin command,
* not just with its name.
plugin call hello n v nv

display x
di "`macro'"
di "$macro"

matlist B
list
