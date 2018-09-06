# init.jl

global_macro = Dict()
scalar = Dict()
matrix = Dict()
var = Dict()
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

printBuffer = IOBuffer()

function getPrintBuffer()
    String(printBuffer)
end


################### USER DEFINED ######################

get_macros = "global1 global2"
set_macros = "global1 global2"
get_matrices = "A B nvm"
set_matrices = "A B nvm"
get_variables = "n v nv"
set_variables = "v nv"
get_scalars = "scalar1"
set_scalars = "scalar1 scalar2"

function nameGetVar(n::Integer)
    names = split(strip(get_variables), r" +")
    if length(names) < n || n < 1
        return("")
    end
    return String(names[n])
end

function isSetVar(str::String)
    names = Set(split(strip(set_variables), r" +"))
    return in(str, names)?1:0;
end

# Can be called to print output in Stata
function printToBuffer()
    # result = var["v"]
    write(printBuffer,"Print Output:\n")
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
