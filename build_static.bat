@echo off
chcp 65001 > nul
rem Лабораторная работа №5, часть 1: статическая библиотека и программа с ней.
rem Результат: build\liblab3.a и build\test_static.exe
rem
rem Сборка выполняется вручную прямыми вызовами g++ и ar - без make, CMake и других
rem систем сборки (требование задания). Каждая команда делает одно действие.
rem Стандарт языка - C++14 (требование преподавателя): флаг -std=c++14.

if not exist build mkdir build

rem 1. Компиляция исходного файла класса в объектный файл (машинный код без точки входа).
rem    -c - только скомпилировать, не линковать; -Iinclude - где искать Polynomial.h.
g++ -std=c++14 -Wall -Wextra -pedantic -Iinclude -c src\Polynomial.cpp -o build\Polynomial.o
if errorlevel 1 goto error

rem 2. Упаковка объектного файла в статическую библиотеку.
rem    ar - архиватор: r - добавить файл, c - создать архив, s - записать индекс символов.
ar rcs build\liblab3.a build\Polynomial.o
if errorlevel 1 goto error

rem 3. Компиляция клиентского кода в объектный файл (код общий для обеих тестовых программ).
g++ -std=c++14 -Wall -Wextra -pedantic -Iinclude -c client\client.cpp -o build\client.o
if errorlevel 1 goto error

rem 4. Линковка: клиент + статическая библиотека -> test_static.exe. Нужный код из библиотеки
rem    копируется внутрь exe, поэтому библиотека на момент запуска не нужна.
rem    Флаг -static встраивает в exe и библиотеки MinGW (libstdc++), чтобы программа
rem    запускалась на компьютере без компилятора.
g++ -static build\client.o build\liblab3.a -o build\test_static.exe
if errorlevel 1 goto error

echo Сборка успешна: build\liblab3.a, build\test_static.exe
exit /b 0

:error
echo Сборка завершилась с ошибкой.
exit /b 1
