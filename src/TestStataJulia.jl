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
    # Varlist obtained from Stata
    # TODO: allow specification also in Julia
    "get_variables"  => "n var1 var2",
    "set_variables" => "var2",
    "set_matrices" => "matfromvar",
    "get_scalars" => "scalar1",
    "set_scalars" => "scalar2",
    "get_macros" => "global1",
    "set_macros" => "global2"
)

function test_rw(stata::StataData)
    x = stata.global_macro["global1"]
    stata.global_macro["global2"] =  "Modified $x"
    stata.scalar["scalar2"] = 2*stata.scalar["scalar1"]
    x = 3 .* stata.variable["n"]
    stata.variable["var2"] = copy(x)
    x = stata.variable["var1"] .* stata.variable["touse"]
    stata.matrix["matfromvar"] = copy(x)
end

# Test the same function, but providing init vars in Stata
function test_rw_stata(stata::StataData)
    test_rw(stata)
end

end
