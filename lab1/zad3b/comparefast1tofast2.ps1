set-location "C:\Users\Adm\desktop\cpp\cpptoasmtests"

echo "compiling"

g++ -S fast1.cpp -o _fast1oN.s
g++ -S fast1.cpp -O1 -o _fast1o1.s
g++ -S fast1.cpp -O2 -o _fast1o2.s
g++ -S fast1.cpp -O3 -o _fast1o3.s
g++ -S fast1.cpp -Os -o _fast1os.s
g++ -S fast1.cpp -Ofast -o _fast1ofast.s
g++ fast1oN.s -o _fast1oNtest.exe
g++ fast1o1.s -o _fast1o1test.exe
g++ fast1o2.s -o _fast1o2test.exe
g++ fast1o3.s -o _fast1o3test.exe
g++ fast1os.s -o _fast1oStest.exe
g++ fast1ofast.s -o _fast1oFastTest.exe

g++ -S fast2.cpp -o _fast2oN.s
g++ -S fast2.cpp -O1 -o _fast2o1.s
g++ -S fast2.cpp -O2 -o _fast2o2.s
g++ -S fast2.cpp -O3 -o _fast2o3.s
g++ -S fast2.cpp -Os -o _fast2os.s
g++ -S fast2.cpp -Ofast -o _fast2ofast.s
g++ fast2oN.s -o _fast2oNtest.exe
g++ fast2o1.s -o _fast2o1test.exe
g++ fast2o2.s -o _fast2o2test.exe
g++ fast2o3.s -o _fast2o3test.exe
g++ fast2os.s -o _fast2oStest.exe
g++ fast2ofast.s -o _fast2oFastTest.exe

echo "adding binaries"

$binarki = [System.Collections.ArrayList]@()
$binarki.add("_fast1oNtest.exe")
$binarki.add("_fast1o1test.exe")
$binarki.add("_fast1o2test.exe")
$binarki.add("_fast1o3test.exe")
$binarki.add("_fast1oStest.exe")
$binarki.add("_fast1oFastTest.exe")

$binarki.add("_fast2oNtest.exe")
$binarki.add("_fast2o1test.exe")
$binarki.add("_fast2o2test.exe")
$binarki.add("_fast2o3test.exe")
$binarki.add("_fast2oStest.exe")
$binarki.add("_fast2oFastTest.exe")

echo "timing"
get-date >> lastlog.txt
echo "fast1.cpp : " >> lastlog.txt
cat fast1.cpp >> lastlog.txt
echo "fast2.cpp : " >> lastlog.txt
cat fast2.cpp >> lastlog.txt
foreach($exeName in $binarki){echo $exeName  |  Tee-Object -FilePath "lastlog.txt" -Append; & Measure-Command {ForEach($item in 1..30) {& .\$exeName}} | Format-Table TotalMilliseconds -auto |  Tee-Object -FilePath "lastlog.txt" -Append}