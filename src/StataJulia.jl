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
    buffer::IOBuffer

end

StataData() = StataData(Dict(), Dict(), Dict(),
DataFrame(), Dict(), Dict(), IOBuffer())


export StataData, putstata, stata

function getInstance()
    st = StataData()
    st.putvars["variables"] = []
    st.putvars["matrices"] = []
    st.putvars["scalars"] = []
    st.putvars["macros"] = []
    st.getvars["variables"] = []
    st.getvars["matrices"] = []
    st.getvars["scalars"] = []
    st.getvars["macros"] = []
    return st
end

function stata(command::String, data::String, selection = "all")
    # 1. Create stata batch file with command using data
    dofile = tempname()
    binfile = tempname()*".bin"
    buf = """
    capture use $data
    global rc = _rc
    if !_rc {
        capture $command
        global rc = _rc
        julia * , save(\"$binfile\") collect($selection)
    }
    else {
        julia , save(\"$binfile\") collect($selection)
    }
    """
    io = open("$dofile.do", "w")
    write(io, buf)
    close(io)

    # 2. Run Stata batch file that invokes julia.ado , all save(tmpfile)
    run(`stata -e -q do $dofile.do`)
    # Delete the log file that Stata creates in the current directory:
    rm((match(r"/([^/]*)$", dofile)[1])*".log")
    # 3. Open serialized tmpfile and return the StataData
    statadata = deserializeData(binfile)
    rm(binfile)
    # Check if global rc has been defined
    # ec = statadata.global_macro["rc"]
    # @assert ec == "0" "Stata returned with error: $ec"
    return statadata
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
function getPrintBuffer(stata::StataData)
    String(take!(stata.buffer))
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

function putstata(stata::StataData, category::String, varname::String)
    push!(stata.putvars[category], varname)
end

function putstata(stata::StataData, category::String, varname::String, value::Any)
    push!(stata.putvars[category], varname)
    if category == "scalars"
        stata.scalar[varname] = Float64(value)
    elseif category == "macros"
        stata.global_macro[varname] = value
        # TODO: String(value)
    elseif category == "variables"
        stata.data[Symbol(varname)] = copy(value) # TODO: Ensure that it is a vector
    elseif category == "matrices"
        stata.matrix[varname] = copy(value[:,:])
    else
        write(stata.buffer, "ERROR: Wrong type")
    end
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
