@echo off 
echo clearing the building temp files, please wait for a while.

del /f /q /s *.obj
del /f /q /s *.pdb
del /f /q /s *.idb
del /f /q /s *.pch
del /f /q /s *.ilk
del /f /q /s *.dll
del /f /q /s *.exe


echo clear finished
pause 