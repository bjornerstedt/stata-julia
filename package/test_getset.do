run test_init.do
*julia , using(init.jl)  function(test_get_set)
julia n v nv, using(test_all_from_stata.jl) function(test_get_set) setvariables(v nv) matrices(A B nvm) setmatrices(A B nvm) scalars(scalar1) setscalars(scalar1 scalar2) macros(global1 global2) setmacros(global1 global2)
matlist A
matlist B
di "$global1"
di "$global2"
di scalar1
di scalar2
list


matlist nvm
