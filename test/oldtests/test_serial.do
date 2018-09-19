run ../test/test_init.do

julia , using(init_serial.jl)  save("test_serial.bin")
*program drop julia
run ../test/test_init.do

julia , using(init_serial.jl)  save("test_serial.bin")
