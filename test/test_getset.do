run ../test/test_init.do

julia , using(init.jl)  function(test_get_set)
matlist A
matlist B
di "$global1"
di "$global2"
di scalar1
di scalar2
list

matlist nvm

julia , using(init.jl)  function(test_get_set)
