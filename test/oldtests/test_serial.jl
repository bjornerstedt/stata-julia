# test_serialization.jl opens saved values

include("init.jl")

io = open("test/test_serial.bin", "r");
stata = deserialize(io)
stata.matrix["A"]
stata.matrix["B"]
print(stata.global_macro["global1"])

a=2
