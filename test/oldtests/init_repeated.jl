# init.jl
include("statajulia.jl")
using DataFrames
using StataJulia

# List variables, matrices, macros and scalars to use
# All variables set must be in the corresponding get
# stata_init["get_variables"]  = "n v nv"
# stata_init["set_variables"] = "v nv"
stata_init["get_matrices"] = "A B"
stata_init["set_matrices"] = "B"
# macros and scalars can be set without predefining them in Stata
# stata_init["get_macros"] = "global1 global2"
# stata_init["set_macros"]  = "global1 global2"
# stata_init["get_scalars"]  = "scalar1"
# stata_init["set_scalars"]  = " scalar2"

# Can be called to print output in Stata
function printToBuffer()
    # result = variable["v"]
    write(printBuffer,"This is my print Output:\n")
    write(printBuffer,"\n")
end

function test_get_set()
    stata.matrix["B"] = transpose(stata.matrix["A"])
    if haskey(stata_init, "set_matrices")
        print(stata_init["set_matrices"])
    else
        print("NOPE")

    end
end

function test_get_set2()
    if haskey(stata_init, "set_matrices")
        print(stata_init["set_matrices"])
    else
        print("NOPE")

    end

    stata.matrix["B"] = transpose(stata.matrix["A"])
    print(stata.matrix["B"])
end
