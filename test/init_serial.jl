# init_serial.jl - Test serializing data from Stata. Run test_serial.do
include("statajulia.jl")
using DataFrames
using StataJulia

# List variables, matrices, macros and scalars to use
# All variables set must be in the corresponding get
stata_init["get_variables"]  = "n v nv"
stata_init["get_matrices"] = "A B nvm"
# macros and scalars can be set without predefining them in Stata
stata_init["get_macros"] = "global1"
stata_init["get_scalars"]  = "scalar1"
