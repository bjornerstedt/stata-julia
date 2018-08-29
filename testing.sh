# Compile Julia interface
gcc -o test -fPIC -I$JULIA_DIR/include/julia -L$JULIA_DIR/lib test.c -ljulia $JULIA_DIR/lib/julia/libstdc++.so.6

# Compile Stata plugin example
gcc -bundle -DSYSTEM=APPLEMAC  -std=gnu99 -DJULIA_ENABLE_THREADING=1 -fPIC -I'/Applications/Julia-0.6.app/Contents/Resources/julia/include/julia' stplugin.c simplecall.c -o simplecall.plugin
gcc -bundle -DSYSTEM=APPLEMAC stplugin.c simplecall.c -o simplecall.plugin
gcc -bundle -DSYSTEM=APPLEMAC stplugin.c hello.c -o hello.plugin

# Comman in julia-embed folder
gcc -o invoke_julia -fPIC -I'/Applications/Julia-0.6.app/Contents/Resources/julia/include/julia' -L'/Applications/Julia-0.6.app/Contents/Resources/julia/lib' invoke_julia.c -ljulia /usr/lib/libstdc++.6.dylib

# Julia config script
/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc invoke_julia.c -o invoke_julia

# TEST combine
/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc -bundle -DSYSTEM=APPLEMAC stplugin.c simplecall.c -o simplecall.plugin


JULIA_BINDIR=/Applications/Julia-0.6.app/Contents/Resources/julia/bin
export JULIA_BINDIR

/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc invoke_julia.c

/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc -bundle -DSYSTEM=APPLEMAC simplecall.c -o simplecall.plugin

gcc  -bundle -DSYSTEM=APPLEMAC -std=gnu99 -I'/Applications/Julia-0.6.app/Contents/Resources/julia/include/julia' -DJULIA_ENABLE_THREADING=1 -fPIC -L'/Applications/Julia-0.6.app/Contents/Resources/julia/lib' -Wl,-rpath,'/Applications/Julia-0.6.app/Contents/Resources/julia/lib' -Wl,-rpath,'/Applications/Julia-0.6.app/Contents/Resources/julia/lib/julia' -ljulia  stplugin.c simplecall.c -o simplecall.plugin
