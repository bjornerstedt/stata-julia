module TestStataJulia2

using StataJulia

function test_rw(stata::StataData)
    x = stata.global_macro["global1"]
    stata.global_macro["global2"] =  "Modified $x"
    stata.scalar["scalar2"] = 2*stata.scalar["scalar1"]
    x = 3 .* stata.data[:n]
    stata.data[:var2] = copy(x)
    x = stata.data[:var1] .* stata.data[:touse]
    stata.matrix["matfromvar"] = copy(x[:,:])

    putstata(stata, "variables", "var2")
    putstata(stata, "macros", "global2")
    putstata(stata, "scalars", "scalar2")
    putstata(stata, "matrices", "matfromvar")
end

function test_get_all(stata::StataData)
    x = stata.global_macro["global1"]
    # stata.global_macro["global2"] =  "Modified $x"
    write(stata.buffer, "global1: $x\n")

    a = stata.getvars["variables"]
    write(stata.buffer, "variables: $a\n")
    a = stata.getvars["scalars"]
    write(stata.buffer, "scalars: $a\n")
    a = stata.getvars["matrices"]
    write(stata.buffer, "matrices: $a\n")

    a = stata.matrix["e_b"]
    write(stata.buffer, "e_b: $a\n")

    # Test values in Stata:
    putstata(stata, "scalars", "newscalar", 3)
    putstata(stata, "matrices", "eb", a)
    putstata(stata, "macros", "newmacro", "Hej")
    putstata(stata, "variables", "var2", stata.data[:var1])
end

end
