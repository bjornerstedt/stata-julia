clear all
capture program drop julia
matrix A = (1,2\3,4)
matrix B = A
global global1 "Hejsan"
global global2 33.3
julia , using(init.jl)  method(transpose_matrix)
di $global1
di $global2
matlist A
matlist B
