@echo off

echo BFS > wynik.txt
Ukladanka.exe -b %1 < %2 >> wynik.txt

echo. >> wynik.txt
echo. >> wynik.txt
echo DFS >> wynik.txt
Ukladanka.exe -d %1 < %2 >> wynik.txt

echo. >> wynik.txt
echo. >> wynik.txt
echo iDFS >> wynik.txt
Ukladanka.exe -i %1 < %2 >> wynik.txt