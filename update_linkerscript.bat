@echo off
REM Скрипт для исправления файла STM32F030F4Px_FLASH.ld
REM Исправляет строку закрытия секции .data в файле STM32F030F4Px_FLASH.ld

setlocal enabledelayedexpansion

set "FILENAME=STM32F030F4Px_FLASH.ld"
set "TMPFILE=%FILENAME%.tmp"

if not exist "%FILENAME%" (
    echo Файл %FILENAME% не найден.
    pause
    exit /b 1
)

REM Обработка построчно с заменой нужной строки
(
    for /F "usebackq delims=" %%a in ("%FILENAME%") do (
        set "line=%%a"
        set "newline=!line!"
        
        REM Проверяем, находимся ли мы в секции .data
        if "!line!" == ".data :" (
            set "in_data_section=1"
        )
        
        REM Если мы в секции .data и нашли закрывающую строку
        if defined in_data_section (
            if "!line:~0,2!" == "} " (
                set "newline=} >RAM AT>FLASH"
                set "in_data_section="
            )
        )
        
        echo !newline!
    )
) > "%TMPFILE%"

move /Y "%TMPFILE%" "%FILENAME%"
echo Файл "%FILENAME%" успешно обновлен.
pause
