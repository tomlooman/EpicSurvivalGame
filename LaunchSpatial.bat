@echo off
echo Starting a local Spatial deployment with the default launch configuration. Modify LaunchSpatial.bat if you want to use a different launch configuration.

pushd "%~dp0\spatial"

spatial worker build build-config 

rem To launch a local Spatial deployment with a different launch configuration, change the default_launch.json argument to the name of your configuration file, 
rem for example one_worker_test.json or two_worker_test.json. Similarly, you can specify a different snapshot than the default one by replacing the value of 
rem the --snapshot argument with the path to the snapshot you want to launch with.
spatial local launch default_launch.json --snapshot=snapshots/default.snapshot

popd
