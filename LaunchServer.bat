@echo off
call "%~dp0FindEngine.bat"
call "%~dp0ProjectPaths.bat"
"%UNREAL_ENGINE:"=%\Engine\Binaries\Win64\UE4Editor.exe" "%~dp0%PROJECT_PATH%\%GAME_NAME%.uproject" -server -log -workerType UnrealWorker -stdout -nowrite -unattended -nologtimes -nopause -noin -messaging -SaveToUserDir -NoVerifyGC -windowed -resX=400 -resY=300
