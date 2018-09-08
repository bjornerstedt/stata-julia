# init.jl
include("statajulia.jl")

using StataJulia

# All variables set in Stata

# Can be called to print output in Stata
function printToBuffer()
    # result = variable["v"]
    write(printBuffer,"This is my print Output:\n")
    write(printBuffer,"\n")
end

function test_get_set()
    matrix["B"] = transpose(matrix["A"])
    global_macro["global1"] = "Test"
    global_macro["global2"] =  "ss"
    scalar["scalar1"] = 2*scalar["scalar1"]
    scalar["scalar2"] = 2.1
    variable["nv"] = 3.*variable["v"]
    matrix["nvm"] = 2.*variable["v"]
end
