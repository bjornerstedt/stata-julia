* Test and exit if
program assertcount
    syntax [anything]

    if ("`anything'" != "") {
        if (`anything') {
            di "Assertion is true"
        }
        else {
            di as error "Assertion is false"
            if ("$errcount" == "") {
                global errcount = 1
            }
            else {
                global errcount = $errcount + 1
            }
        }
    }
    else {
        if ("$errcount" == "") {
            di "***********************************"
            di
            di "******* Passed all tests! *********"
            di
            di "***********************************"
        }
        else {
            di  "$errcount test(s) failed"
        }
    }
end
