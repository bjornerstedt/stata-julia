# init.jl
include("statajulia.jl")

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
stata_init["set_scalars"]  = "scalar1 scalar2"

# Can be called to print output in Stata
function printToBuffer()
    # result = variables["v"]
    write(printBuffer,"This is my print Output:\n")
    write(printBuffer,"\n")
end

function test_get_set()
    matrix["B"] = transpose(matrix["A"])
    global_macro["global1"] = "Test"
    global_macro["global2"] =  "ss"
    scalar["scalar1"] = 2*scalar["scalar1"]
    scalar["scalar2"] = 2.1
    variables["nv"] = 3.*variables["v"]
    matrix["nvm"] = 2.*variables["v"]
end
