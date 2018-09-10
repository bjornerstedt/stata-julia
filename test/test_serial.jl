# test_serialization.jl opens saved values

include("init.jl")

io = open("test/test_serial.bin", "r");
stata = deserialize(io)
print(stata.matrix["A"])
print(stata.global_macro["global1"])
