@echo off
set FILENAME=%1Include\constants.h
FOR /F "tokens=*" %%a in ('git describe --always --dirty --match "NOT A TAG"') do SET COMMIT=%%a
FOR /F "tokens=*" %%b in ('git rev-parse --abbrev-ref HEAD') do SET BRANCH=%%b

echo #pragma once > %FILENAME%
echo. >> %FILENAME%
echo namespace lycoris::constants >> %FILENAME%
echo { >> %FILENAME%
echo    constexpr wchar_t commit_id[] = L"%COMMIT%"; >> %FILENAME%
echo    constexpr wchar_t branch_name[] = L"%BRANCH%"; >> %FILENAME%
echo    constexpr wchar_t configuration_name[] = L"%2"; >> %FILENAME%
echo } >> %FILENAME%
