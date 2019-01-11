$PSScriptRoot = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition

Set-Location -Path "$PSScriptRoot"

if ( Test-Path env:UNREAL_ENGINE_ROOT ) {
    Write-Output "UNREAL_ENGINE_ROOT=$ENV:UNREAL_ENGINE_ROOT"
}
else {
    $ENV:UNREAL_ENGINE_ROOT = "C:/Program Files/Epic Games/UE_4.21"
    Write-Output "UNREAL_ENGINE_ROOT(default)=$ENV:UNREAL_ENGINE_ROOT"
}

$ENV:UNREAL_BUILT_TOOL_BIN = "$ENV:UNREAL_ENGINE_ROOT/Engine/Binaries/DotNET/UnrealBuildTool.exe"
$ENV:UNREAL_RUNUAT = "$ENV:UNREAL_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.bat"

$ENV:PROJECT_ROOT = "$PSScriptRoot"
$ENV:BUILD_LOG_ROOT = "D:"
$ENV:BUILD_TARGET = "Win64"
$ENV:BUILD_OUTPUT_DIR = "$PSScriptRoot/Output"

# & "$ENV:UNREAL_BUILT_TOOL_BIN" SurvivalGame "$ENV:BUILD_TARGET" Development "-Project=$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" "-Manifest=$ENV:PROJECT_ROOT/SurvivalGame/Intermediate/Build/Manifest.xml" -nobuilduht -xgeexport "$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" -NoUBTMakefiles  "-remoteini=$ENV:PROJECT_ROOT/SurvivalGame" -skipdeploy -NoHotReload -log="$ENV:BUILD_LOG_ROOT/UBT-SurvivalGame-Win64-Development.txt"

# & "$ENV:UNREAL_RUNUAT" "-ScriptsForProject=$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" BuildCookRun -nocompile -nocompileeditor -installed -nop4 "-project=$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" -cook -stage -archive "-archivedirectory=$ENV:BUILD_OUTPUT_DIR" -package -clientconfig=Development -ue4exe=UE4Editor-Cmd.exe -pak -prereqs -nodebuginfo "-targetplatform=$ENV:BUILD_TARGET" -build -utf8output

# & "$ENV:UNREAL_RUNUAT" BuildCookRun -project="$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" -noP4 -nosubmit -platform=$ENV:BUILD_TARGET -clientconfig=Development -serverconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory="$ENV:BUILD_OUTPUT_DIR"

& "$ENV:UNREAL_RUNUAT" BuildCookRun -project="$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" -noP4 -nosubmit -platform=$ENV:BUILD_TARGET -targetplatform=$ENV:BUILD_TARGET -clientconfig=DebugGame -serverconfig=DebugGame -cook -allmaps -NoCompile -stage -pak -archive -archivedirectory="$ENV:BUILD_OUTPUT_DIR"

#-ScriptsForProject=D:/workspace/tencent/mtallgroup/EpicSurvivalGameSeries/SurvivalGame/SurvivalGame.uproject BuildCookRun -nocompile -nocompileeditor -installed -nop4 -project=D:/workspace/tencent/mtallgroup/EpicSurvivalGameSeries/SurvivalGame/SurvivalGame.uproject -cook -stage -archive -archivedirectory=D:/workspace/tencent/mtallgroup/EpicSurvivalGameSeries/SurvivalGame/Output -package -clientconfig=Development -ue4exe=UE4Editor-Cmd.exe -pak -prereqs -nodebuginfo -targetplatform=Win64 -build -utf8output
#BuildCookRun -nocompile -nocompileeditor -installed -nop4 -project=D:/workspace/tencent/mtallgroup/EpicSurvivalGameSeries/SurvivalGame/SurvivalGame.uproject -cook -skipstage -ue4exe=UE4Editor-Cmd.exe -targetplatform=Win64 -utf8output
# "-ScriptsForProject=$ENV:BUILD_OUTPUT_DIR/SurvivalGame/SurvivalGame.uproject" BuildCookRun -nocompile -nocompileeditor -installed -nop4 "-project=$ENV:BUILD_OUTPUT_DIR/SurvivalGame/SurvivalGame.uproject" -cook -stage -archive "-archivedirectory=$ENV:BUILD_OUTPUT_DIR/SurvivalGame/Output" -package -clientconfig=DebugGame -ue4exe=UE4Editor-Cmd.exe -pak -prereqs -nodebuginfo -targetplatform=Win64 -build -utf8output

if ( -not $? ) {
    exit 1
}

if ( Test-Path "Package.zip" ) { Remove-Item -Force "Package.zip " }

Set-Location -Path "$ENV:BUILD_OUTPUT_DIR"

Get-ChildItem -Directory | Compress-Archive -DestinationPath ../Package.zip 

Set-Location -Path "$PSScriptRoot"
