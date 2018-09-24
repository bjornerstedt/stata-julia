# test_serialization.jl opens saved values

using StataJulia

df  = StataJulia.deserializeData("test_serial.bin")

@assert df.global_macro["global1"] == "Hello"
@assert df.scalar["scalar1"] == 123.4
@assert df.matrix["A"] == [1.0 3.0; 2.0 4.0]
@assert df.variable["n"][1] == 1.0
