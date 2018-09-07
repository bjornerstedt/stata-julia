# init.jl
module StataJulia

export printBuffer, getPrintBuffer, stata_init, isSetVar
export global_macro, scalar, matrix, variable

printBuffer = IOBuffer()
global_macro = Dict()
stata_init = Dict()
scalar = Dict()
matrix = Dict()
variable = Dict()

function addDataset(y)
    # not implemented
    1
end


function addVariable(x::String, y)
    variable[x] = y
end

function getVariable(x::String)
    variable[x]
end

function addMatrix(x::String, y)
    matrix[x] = y
end

function getMatrix(x::String)
    matrix[x]
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

function isSetVar(str::String)
    names = Set(split(strip(stata_init["set_variables"]), r" +"))
    return in(str, names)?1:0;
end

# To print output in Stata
function getPrintBuffer()
    String(printBuffer)
end

function nameGetVar(n::Integer)
    names = split(strip(stata_init["get_variables"]), r" +")
    if length(names) < n || n < 1
        return("")
    end
    return String(names[n])
end
end
