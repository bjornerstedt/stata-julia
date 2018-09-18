module StataJulia

struct StataData
    global_macro::Dict
    scalar::Dict
    matrix::Dict
    variable::Dict
    # stata_init::Dict
end

# StataData() = StataData(Dict(), Dict(), Dict(), Dict() )
StataData() =  Dict()

export StataData

function getInstance()
    return Dict()
end

function addMatrix(stata::Dict, x::String, y)
    stata[x] = y
    return stata
end

function getMatrix(stata::Dict, x::String)
    # get(stata, x, default)
    stata[x]
end

function addJuliaInitString(stata_init, x, y)
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

#############

getInit() = Dict(
   "get_matrices" => "A B",
   "set_matrices" => "B"
)

function test_get_set(stata::Dict)
    l = length(stata)
    print("Length $l\n")
    # B = transpose(stata["A"])

end

end
