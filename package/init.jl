# init.jl
module StataJulia

export printBuffer, getPrintBuffer, stata_init, isSetVar

printBuffer = IOBuffer()
stata_init = Dict()

function isSetVar(str::String)
    names = Set(split(strip(stata_init["set_variables"]), r" +"))
    return in(str, names)?1:0;
end

# To print output in Stata
function getPrintBuffer()
    String(printBuffer)
end

end

using StataJulia

global_macro = Dict()
scalar = Dict()
matrix = Dict()
var = Dict()

function addMatrix(x::String, y)
    matrix[x] = y
end

function getMatrix(x::String)
    matrix[x]
end

function addVariable(x::String, y)
    var[x] = y
end
function getVariable(x::String)
    var[x]
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

function nameGetVar(n::Integer)
    names = split(strip(stata_init["get_variables"]), r" +")
    if length(names) < n || n < 1
        return("")
    end
    return String(names[n])
end

function addDataset(y)
    dataset = y
end

################### USER DEFINED ######################

stata_init["get_macros"] = "global1 global2"
stata_init["set_macros"]  = "global1 global2"
stata_init["get_matrices"] = "A B nvm"
stata_init["set_matrices"] = "A B nvm"
stata_init["get_variables"]  = "n v nv"
stata_init["set_variables"] = "v nv"
stata_init["get_scalars"]  = "scalar1"
stata_init["set_scalars"]  = "scalar1 scalar2"


# Can be called to print output in Stata
function printToBuffer()
    # result = var["v"]
    write(printBuffer,"This is my print Output:\n")
    write(printBuffer,"\n")
end

function test_get_set()
    matrix["B"] = transpose(matrix["A"])
    global_macro["global1"] = "Test"
    global_macro["global2"] =  "ss"
    scalar["scalar1"] = 2*scalar["scalar1"]
    scalar["scalar2"] = 2.1
    var["nv"] = 3.*var["v"]
    matrix["nvm"] = 2.*var["v"]
end
