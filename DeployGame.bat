@echo off
setlocal ENABLEDELAYEDEXPANSION

:: Get the current date to name the assembly
call "%~dp0ProjectPaths.bat"
set datestr=%date%
set formatdate=%datestr:/=%
:: Remove spaces, and set to all lowercase
set gamename=%GAME_NAME: =%
for %%L IN (a b c d e f g h i j k l m n o p q r s t u v w x y z) DO SET gamename=!gamename:%%L=%%L!
set deploymentname=%gamename%%formatdate%

:: Upload the assembly, and launch the deployment
cd spatial/
spatial build build-config || goto :error
spatial cloud upload %deploymentname% --force || goto :error
spatial cloud launch %deploymentname% one_worker_test.json %deploymentname% --snapshot=snapshots/default.snapshot --cluster_region=eu || goto :error
spatial project deployment tags add %deploymentname% dev_login || goto :error
spatial project deployment tags add %deploymentname% status_lobby || goto :error

echo Deployment succeeded.
cd ../
pause
exit /b 0

:error
echo Deployment failed.
cd ../
pause
exit /b 1
