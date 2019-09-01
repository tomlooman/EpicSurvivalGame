@echo off

call "%~dp0ProjectPaths.bat"

setlocal EnableDelayedExpansion

set GDK_DIRECTORY=""

rem If a project plugin exists. Use this for building.
if exist "%~dp0\%PROJECT_PATH%\Plugins\UnrealGDK" (
    set GDK_DIRECTORY="%~dp0\%PROJECT_PATH%\Plugins\UnrealGDK\"
    goto :BuildWorkers
)

rem If there is no project plugin. Find the engine plugin.
call "%~dp0FindEngine.bat"

if %UNREAL_ENGINE%=="" (
    echo Error: Could not find the Unreal Engine. Please associate your '.uproject' with an engine version or ensure this game project is nested within an engine build.
    pause
    exit /b 1
)

set GDK_DIRECTORY=%UNREAL_ENGINE%\Engine\Plugins\UnrealGDK


:BuildWorkers
echo Building worker with GDK located at %GDK_DIRECTORY%

call %GDK_DIRECTORY%\SpatialGDK\Build\Scripts\BuildWorker.bat %GAME_NAME%Server Linux Development "%~dp0\%PROJECT_PATH%\%GAME_NAME%.uproject" || goto :error
call %GDK_DIRECTORY%\SpatialGDK\Build\Scripts\BuildWorker.bat %GAME_NAME%SimulatedPlayer Linux Development "%~dp0\%PROJECT_PATH%\%GAME_NAME%.uproject" || goto :error
call %GDK_DIRECTORY%\SpatialGDK\Build\Scripts\BuildWorker.bat %GAME_NAME% Win64 Development "%~dp0\%PROJECT_PATH%\%GAME_NAME%.uproject" || goto :error
echo All builds succeeded.

pause
exit /b 0

:error
echo Builds failed.
pause
exit /b 1
