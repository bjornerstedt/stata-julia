using StataJulia
using Test

sd = StataJulia.stata("reg var1 n", "data.dta")
@test sd.global_macro["rc"] == "601"

sd = StataJulia.stata("reg var1 n", "test.dta")
@test sd.matrix["e_b"][1] == 0.40812450882160306
