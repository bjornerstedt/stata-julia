module StataJulia

struct StataData
    global_macro::Dict
    scalar::Dict
    matrix::Dict
    variable::Dict
    # stata_init::Dict
end

StataData() = StataData(Dict(), Dict(), Dict(), Dict() )

export StataData

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
    stata.variable[x] = y
end

function getVariable(stata::StataData, x::String)
    stata.variable[x]
end

function isSetVar(stata_init::Dict, str::String)
    if !haskey(stata_init, "set_variables")
        return 0
    end
    names = Set(split(strip(stata_init["set_variables"]), r" +"))
    return (in(str, names)) ? 1 : 0;
end


function serializeData(stata::StataData, filename::String)
    io = open(filename, "w")
    serialize(io, stata)
    close(io)
end

# TODO: Use function to test serialization
function deserializeData(fn::String)
    io = open(fn, "w")
    stata = deserialize(io)
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

function putstata(x, y)
    print("Init string $x and $y\n")
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

end
