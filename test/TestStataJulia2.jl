module TestStataJulia2

using StataJulia

test_repeated() = Dict(
   "get_matrices" => "A",
   "set_matrices" => "B"
)

function test_repeated(stata::StataData)
    B = transpose(stata.matrix["A"])
    stata.matrix["B"] = copy(B)
end

function test_rw(stata::StataData)
    x = stata.global_macro["global1"]
    stata.global_macro["global2"] =  "Modified $x"
    stata.scalar["scalar2"] = 2*stata.scalar["scalar1"]
    x = 3 .* stata.data[:n]
    stata.data[:var2] = copy(x)
    x = stata.data[:var1] .* stata.data[:touse]
    stata.matrix["matfromvar"] = copy(x[:,:])

    putstata(stata, "variables", "var2")
    putstata(stata, "global_macros", "global2")
    putstata(stata, "scalars", "scalar2")
    putstata(stata, "matrices", "matfromvar")

end

end
