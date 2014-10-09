@echo off

SETLOCAL ENABLEDELAYEDEXPANSION

cd assets/data
if exist asset_list del asset_list
for /f "tokens=*" %%f in ('dir /b /a:-d *.*') do (
	if not %%f==asset_list echo %%f>>asset_list
)

cd images
if exist asset_list del asset_list
for /f "tokens=*" %%f in ('dir /b /a:-d *.*') do (
	if not %%f==asset_list echo %%f>>asset_list
)

cd ../music
if exist asset_list del asset_list
for /f "tokens=*" %%f in ('dir /b /a:-d *.*') do (
	if not %%f==asset_list echo %%f>>asset_list
)

cd ../sounds
if exist asset_list del asset_list
for /f "tokens=*" %%f in ('dir /b /a:-d *.*') do (
	if not %%f==asset_list echo %%f>>asset_list
)