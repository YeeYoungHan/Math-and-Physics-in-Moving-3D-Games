@echo off

REM CH01_Render 폴더의 cpp 파일을 clang-format 으로 실행하려면 아래와 같이 실행하면 됩니다.
REM clangformat.bat CH01_Render\*.cpp
REM

for %%f in (%1) do c:\App\clangformat\clang-format.exe --style=file -i %%f