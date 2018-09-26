module StataJulia

using Serialization
using DataFrames

struct StataData
    global_macro::Dict
    scalar::Dict
    matrix::Dict
    data::DataFrame
    putvars::Dict{String, Set{String}}
    # stata_init::Dict
end

StataData() = StataData(Dict(), Dict(), Dict(),
DataFrame(), Dict(
    "variables" => Set(""),
    "matrices" => Set(""),
    "scalars" => Set(""),
    "global_macros" => Set("")
) )

export StataData, putstata

function getInstance()
    return StataData()
end

function getInit()
    return Dict()
end

function isvalidfunction(x)
    try
        isa(x, Function) ? 1 : 0
    catch
        0
    end
end

function addVariable(stata::StataData, x::String, y)
    stata.data[Symbol(x)] = y
end

function getVariable(stata::StataData, x::String)
    stata.data[Symbol(x)]
end

function putstata(stata::StataData, x::String, y::String)
    push!(stata.putvars[x], y)
end

function isSetVar(stata_init::Dict, str::String)
    if !haskey(stata_init, "set_variables")
        return 0
    end
    names = Set(split(strip(stata_init["set_variables"]), r" +"))
    return (in(str, names)) ? 1 : 0;
end

function isSetVar2(stata::StataData, str::String)
    # if !haskey(stata.putvars, "variables")
    #     return 0
    # end
    return (in(str, stata.putvars["variables"])) ? 1 : 0;
end

function serializeData(stata::StataData, filename::String)
    io = open(filename, "w")
    Serialization.serialize(io, stata)
    close(io)
end

# TODO: Use function to test serialization
function deserializeData(fn::String)
    io = open(fn, "r")
    stata = Serialization.deserialize(io)
    close(io)
    stata
end

# To print output in Stata
function getPrintBuffer()
    String(printBuffer)
end

function addMatrix(stata::StataData, x::String, y)
    stata.matrix[x] = y
end

function getMatrix(stata::StataData, x::String)
    stata.matrix[x]
end

function addScalar(stata::StataData, x::String, y::Number)
    stata.scalar[x] = y
end

function getScalar(stata::StataData, x::String)
    Float64(stata.scalar[x])
end

function addMacro(stata::StataData, x::String, y)
    stata.global_macro[x] = y
end

function getMacro(stata::StataData, x::String)
    stata.global_macro[x]
end

function addJuliaInitString(stata_init::Dict, x::String, y::String)
    stata_init[x] = y
end

function nameGetVar(stata_init::Dict, varlist::String, n::Integer)
    if !haskey(stata_init, varlist)
        return("")
    end
    names = split(strip(stata_init[varlist]), r" +")
    if length(names) < n || n < 1
        return("")
    end
    return String(names[n])
end

function execute_command(cmd)
    a = ""
    try
        a = string(eval(Meta.parse(cmd)))
    catch Error
        a = "ERROR: Could not execute command"
    end
    a
end

end
