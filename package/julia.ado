version 13

program julia , rclass
syntax [varlist] [if] [in], [FUNCtion(name)] [module(name)] [command(string)] [SETVARiables(namelist)] ///
[MATrices(namelist)] [SETMATrices(namelist)] [SCALars(namelist)] [SETSCALars(namelist)] ///
[MACros(namelist)] [SETMACros(namelist)] [save(string)] [all]

if ("`all'" != "") {
    * Copy e() and r() results, to make visible to statajulia
    foreach type in e r {
        local vars : `type'(scalars)
        foreach name in  `vars' {
            scalar `type'_`name' = `type'(`name')
            local scalars `scalars' `type'_`name'
        }
    }
    local vars : all scalars
    foreach name in  `vars' {
        local scalars `scalars' `name'
    }

    foreach type in e r {
        local vars : `type'(matrices)
        foreach name in  `vars' {
            matrix `type'_`name' = `type'(`name')
            local matrices `matrices' `type'_`name'
        }
    }
    local vars : all matrices
    foreach name in  `vars' {
        local matrices `matrices' `name'
    }

    local vars : all globals "M_*"
    foreach name in  `vars' {
        local macros `macros' `name'
    }
}

plugin call statajulia `varlist' `if' `in' , "`function'"  "`module'" "`varlist'" "`command'"  "`save'" ///
 "`setvariables'" "`matrices'" "`setmatrices'" "`scalars'" "`setscalars'" ///
 "`macros'" "`setmacros'"

 *program drop calljulia

end

* TODO: Load different plugin for each OS
program statajulia, plugin using(statajulia.plugin)
