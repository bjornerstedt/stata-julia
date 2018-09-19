module TestStataJulia

using StataJulia

test_get_set() = Dict(
   "get_matrices" => "A",
   "set_matrices" => "B"
)

function test_get_set(stata::StataData)
    B = transpose(stata.matrix["A"])
    stata.matrix["B"] = copy(B)
end

end
