# Setting environment vars:
# export JULIA_DIR=/Applications/Julia-0.6.app/Contents/Resources/julia
# export JULIA_HOME=/Applications/Julia-0.6.app/Contents/Resources/julia/bin
# export JULIA_BINDIR=/Applications/Julia-0.6.app/Contents/Resources/julia/bin

/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc julia_fun.c

./a.out
rm a.out
