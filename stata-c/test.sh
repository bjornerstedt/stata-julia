/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc -bundle -DSYSTEM=APPLEMAC stplugin.c hello.c -o hello.plugin
# Check return value
if [ $? -eq 0 ]
then
  echo "Compilation completed"
else
  echo "There were errors"
  exit
fi
stata < hello.do
rm hello.plugin
