version 13

program julia , rclass
syntax [varlist] [if] [in], [FUNCtion(name)] [module(name)] [command(string)] [SETVARiables(namelist)] ///
[MATrices(namelist)] [SETMATrices(namelist)] [SCALars(namelist)] [SETSCALars(namelist)] ///
[MACros(namelist)] [SETMACros(namelist)] [save(string)] [collect(string)]

if ("`collect'" != "") {
    if ("`collect'" == "all") {
        local ersel "e r"
    }
    else {
        local ersel "`collect'"
    }
    *    if ("`varlist'" == "") {
    *        local varlist *
    *    }
    * Copy e() and r() results, to make visible to statajulia
    foreach type in `ersel' {
        local vars : `type'(scalars)
        foreach name in  `vars' {
            scalar `type'_`name' = `type'(`name')
            local erscalars `erscalars' `type'_`name'
        }
        local vars : `type'(matrices)
        foreach name in  `vars' {
            matrix `type'_`name' = `type'(`name')
            local ermatrices `ermatrices' `type'_`name'
        }
        local vars : `type'(macros)
        foreach name in  `vars' {
            global `type'_`name' = `type'(`name')
            local ermacros `ermacros' `type'_`name'
        }
    }
    local scalars `erscalars'
    local matrices `ermatrices'
    local macros `ermacros'

    local vars : all scalars
    foreach name in  `vars' {
        local scalars `scalars' `name'
    }

    local vars : all matrices
    foreach name in  `vars' {
        local matrices `matrices' `name'
    }

    if ("`collect'" == "all") {
        local vars : all globals
        foreach name in  `vars' {
            local macros `macros' `name'
        }
    }
}

plugin call statajulia `varlist' `if' `in' , "`function'"  "`module'" "`varlist'" "`command'"  "`save'" ///
 "`matrices'" "`scalars'" "`macros'"  ///
 "`setvariables'" "`setmatrices'" "`setscalars'" "`setmacros'"

 if ("`collect'" != "") {
     capture matrix drop `ermatrices'
     capture matrix drop `ermacros'
     capture scalar drop `erscalars'
}

end

* TODO: Load different plugin for each OS
program statajulia, plugin using(statajulia.plugin)
