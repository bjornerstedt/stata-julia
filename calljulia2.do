clear all
capture program drop julia

* Example dataset:
set obs 5
gen n = _n
gen v = exp(n)
gen nv = 0.0

scalar x = 2
local macro Testing!!
global macro Global!!

matrix A = (1,2\3,4)
matrix B = A

di "`macro'"
di "$macro"

* list

* julia n v nv , command(juliafunc)
julia , command(g)

matlist B
