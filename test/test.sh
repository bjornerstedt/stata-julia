
cd ../src
/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc -bundle -DSYSTEM=APPLEMAC stplugin.c statajulia.c calljulia.c basic_functions.c -o ../package/calljulia.plugin
# Check return value
if [ $? -eq 0 ]
then
  echo "Compilation completed"
else
  echo "There were errors!"
  exit
fi

cd ../test
cp ../package/calljulia.plugin .
cp ../package/julia.ado .
cp ../package/statajulia.jl .

stata < test.do
