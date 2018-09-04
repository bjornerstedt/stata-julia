clear all
capture program drop julia

* Example dataset:
set obs 5
gen n = _n
gen v = exp(n)
gen nv = 0.0

scalar x = 2

matrix A = (1,2\3,4)

matrix B = A
julia , function(identity) using(init.jl)
matlist B

matrix A = (1,2\3,4\5,6)

matrix B = A
julia , function(g) using(init.jl)
matlist B

matrix A = (1,2,3,4)

matrix B = A
julia , command(test) using(init.jl)  function(g)
matlist B

julia , command(a=2+2)
julia , command(2+2.0)
di r(command)
