run test_init.do
julia , using(init.jl)  function(test_get_set)
matlist A
matlist B
di "$global1"
di "$global2"
di scalar1
di scalar2
list
* julia n v nv, using(init.jl)  function(test_get_set)
matlist nvm