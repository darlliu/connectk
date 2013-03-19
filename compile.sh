cd src
echo "building project, for team Yu/Afshin"
echo "attempting module load"
module load gcc/4.7.1
echo "Commencing build, GCC ver is:"
g++ -v;
echo "checking ld has libm"
ldconfig -v | grep "libm.so"
echo "checking ld has libc"
ldconfig -v | grep "libc.so"

echo "Please make sure gcc version is above 4.7!!! Otherwise build WILL FAIL"
g++ -v -o ConKuerer.exe main.cpp master.cpp headers.h master.h utils.h -O3 -std=c++11
echo "Please do not unload gcc/4.7.1 because the program is now linked dynamically"
mv ConKuerer.exe ../
cd ..
ls | grep ConKuerer
