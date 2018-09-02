clear all
capture program drop julia
matrix A = (1,2,3,4)
matrix B = A
global global1 "Hejsan"
global global2 33.3
* matlist B
julia , using(init.jl)  method(g)
di $global1
di $global2
