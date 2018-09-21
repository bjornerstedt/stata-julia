
cd ../package/src
$JULIA_HOME/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc -bundle -DSYSTEM=APPLEMAC stplugin.c statajulia.c calljulia.c basic_functions.c -o ../statajulia.plugin
# Check return value
if [ $? -eq 0 ]
then
  echo "Compilation completed"
else
  echo "There were errors!"
  exit
fi

cd ../../test
cp ../package/statajulia.plugin .
cp ../package/julia.ado .

stata < test.do
