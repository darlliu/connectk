cd src
echo "building project, for team Yu/Afshin"
echo "attempting module load"
module load gcc/4.7.1
echo "Commencing build, GCC ver is:"
g++ -v;
echo "Please make sure gcc version is above 4.7!!! Otherwise build WILL FAIL"
g++ -o ConKuerer.exe main.cpp master.cpp headers.h master.h utils.h -O3 -std=c++11 -static
echo "at this point you can unload gcc/4.7.1"
mv ConKuerer.exe ../
cd ..
ls | grep ConKuerer
