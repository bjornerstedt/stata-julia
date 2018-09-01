clear all
capture program drop julia
matrix A = (1,2,3,4)
matrix B = A
julia , command(test) using(init.jl)  method(g)
matlist B
