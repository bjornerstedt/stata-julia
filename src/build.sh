/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc -bundle -DSYSTEM=APPLEMAC stplugin.c statajulia.c calljulia.c basic_functions.c -o ../package/calljulia.plugin
# Check return value
if [ $? -eq 0 ]
then
  echo "Compilation completed"
else
  echo "There were errors"
  exit
fi

cd ../package
stata < ../src/test/test_getset.do
# stata < test_simple.do
cd ../src

rm ../package/juliacall.plugin
