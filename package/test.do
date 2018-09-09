* test.do provides some examples how the julia command can be usedd in Stata

clear all
capture program drop julia
capture program drop calljulia

* Generate example dataset:
set obs 5
gen n = _n
gen v = exp(n)
gen nv = 0.0
mkmat nv , matrix( nvm)

* Generate other data
matrix A = (1,2\3,4)
* Matrix B will hold the transpose of A
matrix B = A
scalar scalar1 = 123.4

julia , using(init.jl)  function(test_get_set)

matlist A
matlist B
di "$global1"
di "$global2"

* Return scalar2 = 2*scalar1
di scalar1
di scalar2

* Variable nv = 4*v
list
