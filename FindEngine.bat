@echo off

setlocal EnableDelayedExpansion

rem Get the Unreal Engine used by this project by querying the registry for the engine association found in the .uproject.
set UNREAL_ENGINE=""
set UPROJECT=""

rem First find the .uproject
for /f "delims=" %%A in (' powershell -Command "Get-ChildItem %~dp0 -Depth 1 -Filter *.uproject -File | %% {$_.FullName}" ') do set UPROJECT="%%A"

if %UPROJECT%=="" (
    echo Error: Could not find uproject. Please make sure you have passed in the project directory correctly.
    pause
    exit /b 1
)

echo Using uproject: %UPROJECT%

rem Get the Engine association from the uproject.
for /f "delims=" %%A in (' powershell -Command "(Get-Content %UPROJECT% | ConvertFrom-Json).EngineAssociation" ') do set ENGINE_ASSOCIATION=%%A

echo Engine association for uproject is: %ENGINE_ASSOCIATION%

rem If the engine association is a path then use this. If the path is relative then it will be relative to the uproject, thus we must change directory to the uproject folder.

rem Grab the project path from the .uproject file.
for %%i in (%UPROJECT%) do (
	rem file drive + file directory
	set UNREAL_PROJECT_DIR="%%~di%%~pi"
)

pushd %UNREAL_PROJECT_DIR%

if exist "%ENGINE_ASSOCIATION%" (
    cd /d "%ENGINE_ASSOCIATION%"
    set UNREAL_ENGINE="!cd!"
)

popd

rem Try and use the engine association as a key in the registry to get the path to Unreal.
if %UNREAL_ENGINE%=="" (
    if not "%ENGINE_ASSOCIATION%"=="" (
        rem Query the registry for the path to the Unreal Engine using the engine association.
        for /f "usebackq tokens=1,2* skip=2" %%A in (`reg query "HKCU\Software\Epic Games\Unreal Engine\Builds" /v %ENGINE_ASSOCIATION%`) do (
            set UNREAL_ENGINE="%%C"
        )
    )
)

rem If there was no engine association then we need to climb the directory path of the project to find the Engine.
if %UNREAL_ENGINE%=="" (
    pushd "%~dp0"

    :climb_parent_directory
    if exist Engine (
        rem Check for the Build.version file to be sure we have found a correct Engine folder.
        if exist "Engine\Build\Build.version" (
            set UNREAL_ENGINE="!cd!"
        )
    ) else (
        rem This checks if we are in a root directory. If so we cannot check any higher and so should error out.
        if "%cd:~3,1%"=="" (
            echo Error: Could not find Unreal Engine folder. Please set a project association or ensure your game project is within an Unreal Engine folder.
            pause
            exit /b 1
        )
        cd ..
        goto :climb_parent_directory
    )

    popd
)

if %UNREAL_ENGINE%=="" (
    echo Error: Could not find the Unreal Engine. Please associate your '.uproject' with an engine version or ensure this game project is nested within an engine build.
    pause
    exit /b 1
)

endlocal & set UNREAL_ENGINE=%UNREAL_ENGINE%

echo Unreal engine found at: %UNREAL_ENGINE%