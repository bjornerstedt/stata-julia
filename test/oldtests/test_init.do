
clear all
capture program drop julia
capture program drop calljulia

* Example dataset:
set obs 5
gen n = _n
gen v = exp(n)
gen nv = 0.0
mkmat nv , matrix( nvm)

matrix A = (1,2\3,4)
matrix B = A
scalar scalar1 = 123.4
scalar scalar2 = 0
global global1 "Hejsan"
