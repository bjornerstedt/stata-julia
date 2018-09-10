run ../test/test_init.do

julia , using(init_serial.jl)  save("test_serial.bin")



*julia , using(init.jl)  function(test_get_set)
