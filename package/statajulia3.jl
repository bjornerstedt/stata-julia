# init.jl
module StataJulia

# using DataFrames

export printBuffer, getPrintBuffer, stata_init, isSetVar
export global_macro, scalar, matrix, variable
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

# global_macro = Dict()
# scalar = Dict()
# matrix = Dict()
# variable = Dict()
# dataset = DataFrame()

# function addToDataset(x::String, y::Array)
#     dataset[Symbol.(x)] = y
# end
#
# function getFromDataset(x::String)
#     dataset[Symbol.(x)]
# end

function addVariable(x::String, y)
    stata.variable[x] = y
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

function addJuliaInitString(x, y)
    stata_init[x] = y
end

end
