* Test calling statajulia
*   - command option
*   - serialising in Julia
*   - setting with Stata options instead of in Julia
*   - printing output to buffer

forvalues i = 1/10 {
    di "Julia ivocation: `i'"
    julia , command(sin(2))
}
julia , command(sin(2))

julia , command(urgh)
julia , command(")(urgh")

* TODO; Return value to test.
julia , module(TestStataJulia)  function(urg)
julia , module(TestStataJulia)  function(bad_function)
