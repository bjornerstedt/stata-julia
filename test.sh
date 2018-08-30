/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc -bundle -DSYSTEM=APPLEMAC stplugin.c calljulia.c -o calljulia.plugin

stata < calljulia.do
rm calljulia.plugin
