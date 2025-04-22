# PowerShell-скрипт: копируем STM32F030F4Px_FLASH.ld_ → STM32F030F4Px_FLASH.ld
# Файл должен быть сохранён в кодировке UTF-8 (лучше UTF-8-BOM)

# Гарантируем вывод в UTF-8
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

$ldFile = "STM32F030F4Px_FLASH.ld"
$ldFileNew = "STM32F030F4Px_FLASH.ld_"

if (Test-Path -Path $ldFileNew) {
    if (Test-Path -Path $ldFile) {
        Remove-Item -Path $ldFile -Force
    }
    
    Copy-Item -Path $ldFileNew -Destination $ldFile -Force
    
    Write-Host "Файл $ldFile скопирован из $ldFileNew. Исходный файл с подчёркиванием сохранён."
} else {
    Write-Host "Файл $ldFileNew не найден. Копирование не выполнено."
}
