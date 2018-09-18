module SJExample

using statajulia

test_get_set() = Dict(
   "get_matrices" => "A B",
   "set_matrices" => "B"
)

function test_get_set(stata::StataData)
    stata.matrix["B"] = transpose(stata.matrix["A"])
end

end
