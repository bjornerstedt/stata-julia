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

test_rw() = Dict(
    "get_variables"  => "n var1",
    "set_variables" => "var2",
    "get_scalars" => "scalar1",
    "set_scalars" => "scalar2",
    "get_macros" => "global1",
    "set_macros" => "global2"
)

function test_rw(stata::StataData)
    stata.global_macro["global1"] = "Test"
    # stata.global_macro["global2"] =  "Second string"
    # stata.scalar["scalar2"] = 2*stata.scalar["scalar1"]
    # x = 3.*stata.variable["var1"]
    # stata.variable["var2"] = copy(x)
    # x = 2.*stata.variable["v"].*stata.variable["touse"]
    # stata.matrix["nvm"] = copy(x)
end

end
