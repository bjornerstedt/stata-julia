module TestStataJulia

using StataJulia

test_repeated() = Dict(
   "get_matrices" => "A",
   "set_matrices" => "B"
)

function test_repeated(stata::StataData)
    B = transpose(stata.matrix["A"])
    stata.matrix["B"] = copy(B)
end

init() = Dict(
    # Varlist obtained from Stata
    # TODO: allow specification also in Julia
    # "set_variables" => "var2",
    # "set_matrices" => "matfromvar",
    "get_scalars" => "scalar1",
    "set_scalars" => "scalar2",
    "get_macros" => "global1",
    "set_macros" => "global2"
)

function test_rw(stata::StataData)
    x = stata.global_macro["global1"]
    stata.global_macro["global2"] =  "Modified $x"
    stata.scalar["scalar2"] = 2*stata.scalar["scalar1"]
    x = 3 .* stata.data[:n]
    stata.data[:var2] = copy(x)
    x = stata.data[:var1] .* stata.data[:touse]
    stata.matrix["matfromvar"] = copy(x[:,:])

    # Write to stata.buffer to get output printed in Stata
    write(stata.buffer, "This is my print Output:\n")

    putstata(stata, "variables", "var2")
    putstata(stata, "global_macros", "global2")
    putstata(stata, "scalars", "scalar2")
    putstata(stata, "matrices", "matfromvar")

end

end
