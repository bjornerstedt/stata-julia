# init.jl

global_macro = Dict()
scalar = Dict()
matrix = Dict()
variable = Dict() # Create a data frame instead
dataset = []

function addMatrix(x::String, y)
    matrix[x] = y
end

function getMatrix(x::String)
    matrix[x]
end

function addDataset(y)
    dataset = y
end

function getVariable(x::String)
    variable[x]
end

function addScalar(x::String, y)
    scalar[x] = y
end

function getScalar(x::String)
    Float64(scalar[x])
end

function addMacro(x::String, y)
    global_macro[x] = y
end

function getMacro(x::String)
    global_macro[x]
end

################### USER DEFINED ######################

get_macros = "global1 global2"
set_macros = "global1 global2"
get_matrices = "A B"
set_matrices = "A B"
get_variables = "n v nv"
set_variables = "nv"
get_scalars = "scalar1"
set_scalars = "scalar1 scalar2"

function test_get_set()
    matrix["B"] = transpose(matrix["A"])
    global_macro["global1"] = "Test"
    global_macro["global2"] =  "ss"
    scalar["scalar1"] = 2*scalar["scalar1"]
    scalar["scalar2"] = 2.1
end
