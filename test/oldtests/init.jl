# init.jl
include("statajulia.jl")
using DataFrames
using StataJulia

# List variables, matrices, macros and scalars to use
# All variables set must be in the corresponding get
stata_init["get_variables"]  = "n v nv"
stata_init["set_variables"] = "v nv"
stata_init["get_matrices"] = "A B nvm"
stata_init["set_matrices"] = "A B nvm"
# macros and scalars can be set without predefining them in Stata
stata_init["get_macros"] = "global1 global2"
stata_init["set_macros"]  = "global1 global2"
stata_init["get_scalars"]  = "scalar1"
stata_init["set_scalars"]  = "scalar2"

# Can be called to print output in Stata
function printToBuffer()
    # result = variable["v"]
    write(printBuffer,"This is my print Output:\n")
    write(printBuffer,"\n")
end

function test_get_set()
    # df = DataFrame()
    # a = variable["v"]
    # print( a)
    # dataset[Symbol("v")] = [1; 2]
    # print(dataset)
    # # TODO: This does not work
    # df[Symbol("v")] = matrix["nvm"]
    stata.matrix["B"] = transpose(stata.matrix["A"])
    stata.global_macro["global1"] = "Test"
    stata.global_macro["global2"] =  "Second string"
    stata.scalar["scalar2"] = 2*stata.scalar["scalar1"]
    stata.variable["nv"] = 3.*stata.variable["v"]
    stata.matrix["nvm"] = 2.*stata.variable["v"].*stata.variable["touse"]
end
