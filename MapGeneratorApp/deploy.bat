:: Deployment script for Disciples II random scenario generator
:: Usage: place script and rsg.exe in the same (preferably empty) folder
:: Run script
:: Zip results and send to users

:: Update path so windeployqt works correctly
PATH=C:\Qt\5.13.0\msvc2017\bin;%PATH%

:: Go to Qt binaries folder
cd C:\Qt\5.13.0\msvc2017\bin

:: Run deployment tool, assuming rsg.exe exists in the same folder as script 
windeployqt^
 --release^
 --no-webkit2^
 --no-system-d3d-compiler^
 --no-opengl^
 --no-opengl-sw^
 --no-angle^
 --no-svg^
 --no-quick-import^
 --no-translations^
 %~dp0\rsg.exe

