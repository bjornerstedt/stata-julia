# init.jl
include("statajulia.jl")
using DataFrames
using StataJulia

# stata = StataData(Dict(), Dict(), Dict(), Dict(), Dict()  )

function initParams()
    stata_init = Dict()
    stata_init["get_matrices"] = "A B"
    stata_init["set_matrices"] = "B"
    return stata_init
end


function test_get_set(stata::Dict)
    # function test_get_set(stata::StataData)
    # stata.matrix["B"] = transpose(stata.matrix["A"])
    stata["B"] = transpose(stata["A"])
    # if haskey(stata, "A")
    #     print(stata["A"] )
    # else
    #     print("NOPE")
    # end
end

function test_get_set2()
    print("HÅ")
end
