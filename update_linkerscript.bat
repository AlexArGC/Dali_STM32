@echo off
REM Скрипт для автоматического исправления секций в файле STM32F030F4Px_FLASH.ld с учетом проблем кодировки
REM Вносит следующие изменения:
REM 1. В секции .data: "} > AT> FLASH" -> "} >RAM AT>FLASH"
REM 2. В секции .bss: "} >" -> "} >RAM"
REM 3. В секции ._user_heap_stack: "} >" -> "} >RAM"

setlocal

set "FILENAME=STM32F030F4Px_FLASH.ld"
set "TMPFILE=%FILENAME%.tmp"

if not exist "%FILENAME%" (
    echo Файл %FILENAME% не найден.
    pause
    exit /b 1
)

REM Используем PowerShell для корректной обработки файла с кодировкой UTF-8
powershell -NoProfile -Command ^
  "$file = '%FILENAME%';" ^
  "$output = Get-Content -Encoding UTF8 $file | ForEach-Object {" ^
  "    if ($_ -match '^\s*\.data\s*:') { $section = 'data' }" ^
  "    elseif ($_ -match '^\s*\.bss\s*:') { $section = 'bss' }" ^
  "    elseif ($_ -match '^\s*\._user_heap_stack\s*:') { $section = 'heap' }" ^
  "    if ($section -eq 'data' -and $_ -match '^\s*\}\s*>') {" ^
  "         $_ = '} >RAM AT>FLASH';" ^
  "         $section = $null;" ^
  "    } elseif (($section -eq 'bss' -or $section -eq 'heap') -and $_ -match '^\s*\}\s*>') {" ^
  "         $_ = '} >RAM';" ^
  "         $section = $null;" ^
  "    }" ^
  "    $_" ^
  "};" ^
  "Set-Content -Encoding UTF8 -Path '%TMPFILE%' -Value $output;"

move /Y "%TMPFILE%" "%FILENAME%"
echo Файл "%FILENAME%" успешно обновлен.
