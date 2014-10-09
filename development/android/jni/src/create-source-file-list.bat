@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

cd ../../../..

for /f "tokens=*" %%f in ('dir /b *.cpp') do (
	echo ../../../../%%f \>>development/android/jni/src/source_file_list
)