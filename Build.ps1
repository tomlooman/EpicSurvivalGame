$PSScriptRoot = Split-Path -Parent -Path $MyInvocation.MyCommand.Definition

cd "$PSScriptRoot"

$ENV:UNREAL_ENGINE_ROOT = "C:/Program Files/Epic Games/UE_4.21"
$ENV:UNREAL_BUILT_TOOL_BIN = "$ENV:UNREAL_ENGINE_ROOT/Engine/Binaries/DotNET/UnrealBuildTool.exe"
$ENV:UNREAL_RUNUAT = "$ENV:UNREAL_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.bat"

$ENV:PROJECT_ROOT = "$PSScriptRoot"
$ENV:BUILD_LOG_ROOT = "D:"
$ENV:BUILD_TARGET = "Win64"
$ENV:BUILD_OUTPUT_DIR = "D:/workspace/tencent/mtallgroup/EpicSurvivalGameSeries/SurvivalGame/Output"

# & "$ENV:UNREAL_BUILT_TOOL_BIN" SurvivalGame "$ENV:BUILD_TARGET" Development "-Project=$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" "-Manifest=$ENV:PROJECT_ROOT/SurvivalGame/Intermediate/Build/Manifest.xml" -nobuilduht -xgeexport "$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" -NoUBTMakefiles  "-remoteini=$ENV:PROJECT_ROOT/SurvivalGame" -skipdeploy -NoHotReload -log="$ENV:BUILD_LOG_ROOT/UBT-SurvivalGame-Win64-Development.txt"

# & "$ENV:UNREAL_RUNUAT" "-ScriptsForProject=$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" BuildCookRun -nocompile -nocompileeditor -installed -nop4 "-project=$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" -cook -stage -archive "-archivedirectory=$ENV:BUILD_OUTPUT_DIR" -package -clientconfig=Development -ue4exe=UE4Editor-Cmd.exe -pak -prereqs -nodebuginfo "-targetplatform=$ENV:BUILD_TARGET" -build -utf8output

& "$ENV:UNREAL_RUNUAT" BuildCookRun -project="$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" -noP4 -nosubmit -platform=$ENV:BUILD_TARGET -clientconfig=Development -serverconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory="$ENV:BUILD_OUTPUT_DIR"

# & "$ENV:UNREAL_RUNUAT" BuildCookRun -project="$ENV:PROJECT_ROOT/SurvivalGame/SurvivalGame.uproject" -noP4 -nosubmit -platform=$ENV:BUILD_TARGET -clientconfig=Development -serverconfig=Development -cook -allmaps -NoCompile -stage -pak -archive -archivedirectory="$ENV:BUILD_OUTPUT_DIR"

#-ScriptsForProject=D:/workspace/tencent/mtallgroup/EpicSurvivalGameSeries/SurvivalGame/SurvivalGame.uproject BuildCookRun -nocompile -nocompileeditor -installed -nop4 -project=D:/workspace/tencent/mtallgroup/EpicSurvivalGameSeries/SurvivalGame/SurvivalGame.uproject -cook -stage -archive -archivedirectory=D:/workspace/tencent/mtallgroup/EpicSurvivalGameSeries/SurvivalGame/Output -package -clientconfig=Development -ue4exe=UE4Editor-Cmd.exe -pak -prereqs -nodebuginfo -targetplatform=Win64 -build -utf8output
#BuildCookRun -nocompile -nocompileeditor -installed -nop4 -project=D:/workspace/tencent/mtallgroup/EpicSurvivalGameSeries/SurvivalGame/SurvivalGame.uproject -cook -skipstage -ue4exe=UE4Editor-Cmd.exe -targetplatform=Win64 -utf8output
