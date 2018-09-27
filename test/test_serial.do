* Test calling statajulia
*   - command option
*   - serialising in Julia
*   - setting with Stata options instead of in Julia
*   - printing output to buffer

run initvars.do


* Save file
julia * ,  save("test_serial.bin") mat(A B) mac(global1) scal(scalar1)

* Error thrown if tests do not pass:
!julia test_serial.jl
