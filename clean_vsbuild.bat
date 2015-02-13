@echo off
echo 清除所有obj pch idb pdb ncb opt plg res sbr ilk 文件，请稍等......
pause
del /f /s /q .\*.obj
del /f /s /q .\*.pch
del /f /s /q .\*.idb
del /f /s /q .\*.pdb
del /f /s /q .\*.ncb 
del /f /s /q .\*.opt 
del /f /s /q .\*.plg
del /f /s /q .\*.res
del /f /s /q .\*.sbr
del /f /s /q .\*.ilk
del /f /s /q .\*.dep
del /f /s /q .\*.manifest
del /f /s /q .\*.ib_tag
del /f /s /q .\*.ib_pdb_index
del /f /s /q buildlog.htm


echo 清除文件完成！
echo. & pause