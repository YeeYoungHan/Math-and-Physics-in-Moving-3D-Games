@echo off

REM CH01_Render ������ cpp ������ clang-format ���� �����Ϸ��� �Ʒ��� ���� �����ϸ� �˴ϴ�.
REM clangformat.bat CH01_Render\*.cpp
REM

for %%f in (%1) do c:\App\clangformat\clang-format.exe --style=file -i %%f