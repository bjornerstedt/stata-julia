module StataJulia

using Serialization
using DataFrames

struct StataData
    global_macro::Dict
    scalar::Dict
    matrix::Dict
    data::DataFrame
    getvars::Dict{String, Array{String}}
    putvars::Dict{String, Array{String}}
    # stata_init::Dict
end

StataData() = StataData(Dict(), Dict(), Dict(),
DataFrame(), Dict(), Dict())


export StataData, putstata

function getInstance()
    st = StataData()
    st.putvars["variables"] = []
    st.putvars["matrices"] = []
    st.putvars["scalars"] = []
    st.putvars["global_macros"] = []
    st.getvars["variables"] = []
    st.getvars["matrices"] = []
    st.getvars["scalars"] = []
    st.getvars["global_macros"] = []
    return st
end

function getInit()
    return Dict( )
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

function copyInitVars(stata::StataData, stata_init::Dict)
    for (key, value) in stata_init
        names = Array{String}(split(strip(value), r" +"))
        listname = SubString(key, 5)
        if SubString(key, 1, 3) == "set"
            stata.putvars[listname] = names
        else
            stata.getvars[listname] = names
        end
    end
end

function putstata(stata::StataData, x::String, y::String)
    push!(stata.putvars[x], y)
end

function isSetVar(stata::StataData, var::String, str::String)
    if !haskey(stata.putvars, var)
        return 0
    end
    return (in(str, stata.putvars[var])) ? 1 : 0;
end

function nameGetVar(stata::StataData, varlist::String, n::Integer)
    if !haskey(stata.getvars, varlist)
        return("")
    end
    if length(stata.getvars[varlist]) < n || n < 1
        return("")
    end
    return String(stata.getvars[varlist][n])
end

function namePutVar(stata::StataData, varlist::String, n::Integer)
    if !haskey(stata.putvars, varlist)
        return("")
    end
    if length(stata.putvars[varlist]) < n || n < 1
        return("")
    end
    return String(stata.putvars[varlist][n])
end

function nameGetVar2(stata_init::Dict, varlist::String, n::Integer)
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
        a = "ERROR: Could not execute command: $cmd"
    end
    a
end

end
