module StataExample

using StataJulia

function example_func(stata::StataData)
    s = stata.scalar["scalar1"]
    putstata(stata, "scalars", "scalar2", 2 * s)
    
    putstata(stata, "macros", "newmacro", "Hello")

    m = transpose(stata.matrix["A"])
    putstata(stata, "matrices", "B", m)
end

end
