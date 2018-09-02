# init.jl


global_macro = Dict()
matrix = Dict()
dataset = Dict() # Create a data frame instead

function addMacro(x::String, y)
    global_macro[x] = y
end

function getMacro(x::String)
    global_macro[x]
end

function addMatrix(x::String, y)
    matrix[x] = y
end

function getMatrix(x::String)
    matrix[x]
end

################### USER DEFINED ######################

get_macros = "global1 global2"
set_macros = "global1 global2"
get_matrices = "A B"
set_matrices = "A B"
get_variables = "A B"
set_variables = "A B"

function transpose_matrix()
    matrix["B"] = transpose(matrix["A"])
end
