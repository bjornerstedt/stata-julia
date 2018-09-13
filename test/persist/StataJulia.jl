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

function addMatrix(stata::StataData, x::String, y)
    stata.matrix[x] = y
end

function getMatrix(stata::StataData, x::String)
    stata.matrix[x]
end

function addJuliaInitString(x, y)
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

test_get_set() = Dict(
   "get_matrices" => "A B",
   "set_matrices" => "B"
)

function test_get_set(stata::StataData)
    stata.matrix["B"] = transpose(stata.matrix["A"])
end

end
