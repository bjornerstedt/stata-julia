# init.jl
module StataJulia

# using DataFrames

export serializeData, deserializeData
export printBuffer, getPrintBuffer, stata_init, isSetVar
# export dataset
export stata

struct StataData
    global_macro::Dict
    scalar::Dict
    matrix::Dict
    variable::Dict
end

stata = StataData(Dict(), Dict(), Dict(), Dict() )

# To print in Stata Console
printBuffer = IOBuffer()
stata_init = Dict()

# function addToDataset(x::String, y::Array)
#     dataset[Symbol.(x)] = y
# end
#
# function getFromDataset(x::String)
#     dataset[Symbol.(x)]
# end

stata_init["get_variables"]  = ""
stata_init["set_variables"] = ""
stata_init["get_matrices"] = ""
stata_init["set_matrices"] = ""
stata_init["get_macros"] = ""
stata_init["set_macros"]  = ""
stata_init["get_scalars"]  = ""
stata_init["set_scalars"]  = ""

function getInstance()
    StataData(Dict(), Dict(), Dict(), Dict() )
end

function destructor()
    stata_init  = nothing
    stata = nothing
    printBuffer = nothing
end

function addVariable(x::String, y)
    stata.variable[x] = y
end

function serializeData()
    io = open(stata_init["savefile"], "w")
    serialize(io, stata)
    close(io)
end

# TODO: Use function to test serialization
function deserializeData(fn::String)
    io = open(fn, "w")
    stata = deserialize(io)
    close(io)
end

function getVariable(x::String)
    stata.variable[x]
end

function addMatrix(x::String, y)
    stata.matrix[x] = y
end

function getMatrix(x::String)
    stata.matrix[x]
end

function addScalar(x::String, y)
    stata.scalar[x] = y
end

function getScalar(x::String)
    Float64(stata.scalar[x])
end

function addMacro(x::String, y)
    stata.global_macro[x] = y
end

function getMacro(x::String)
    stata.global_macro[x]
end

function isSetVar(str::String)
    if !haskey(stata_init, "set_variables")
        return 0
    end
    names = Set(split(strip(stata_init["set_variables"]), r" +"))
    return (in(str, names))?1:0;
end

# To print output in Stata
function getPrintBuffer()
    String(printBuffer)
end

function nameGetVar(n::Integer)
    if !haskey(stata_init, "get_variables")
        return("")
    end
    names = split(strip(stata_init["get_variables"]), r" +")
    if length(names) < n || n < 1
        return("")
    end
    return String(names[n])
end

function addJuliaInitString(x, y)
    stata_init[x] = y
end

end
