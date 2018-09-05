clear all
capture program drop julia
* Example dataset:
set obs 5
gen n = _n
gen v = exp(n)
gen nv = 0.0

matrix A = (1,2\3,4)
matrix B = A
scalar scalar1 = 123.4
global global1 "Hejsan"
global global2 33.3
julia , using(init.jl)  function(test_get_set)
di "$global1"
di "$global2"
matlist A
matlist B
di scalar1
di scalar2

* julia n v nv, using(init.jl)  function(test_get_set)
