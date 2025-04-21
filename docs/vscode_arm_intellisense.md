# Настройка Intellisense для ARM Makefile проекта в Visual Studio Code

## 1. Установите необходимые расширения

- [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) (от Microsoft)
- [Makefile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools) (от Microsoft)
- (Опционально) Cortex-Debug для отладки STM32/ARM

## 2. Создайте файл конфигурации Intellisense

В корне проекта создайте папку `.vscode` и файл `c_cpp_properties.json`:

```json
{
    "configurations": [
        {
            "name": "ARM STM32",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/Core/Inc",
                "${workspaceFolder}/Drivers/**"
            ],
            "defines": [
                "STM32F1", // или ваш чип
                "USE_HAL_DRIVER"
            ],
            "compilerPath": "/usr/bin/arm-none-eabi-gcc",
            "cStandard": "c11",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-arm",
            "browse": {
                "path": [
                    "${workspaceFolder}/Core/Inc",
                    "${workspaceFolder}/Drivers/**"
                ],
                "limitSymbolsToIncludedHeaders": true
            }
        }
    ],
    "version": 4
}
```

**Пояснения:**
- `compilerPath` — путь к вашему компилятору ARM GCC (уточните, где установлен arm-none-eabi-gcc).
- `includePath` — пути к заголовочным файлам проекта и библиотек.
- `defines` — ваши макросы (см. Makefile и stm32fxxx.h).

## 3. Настройте Makefile Tools

В `.vscode/settings.json` добавьте:

```json
{
    "makefile.makePath": "/usr/bin/make",
    "makefile.configurations": [
        {
            "name": "default",
            "makeArgs": []
        }
    ],
    "makefile.launchConfigurations": [
        {
            "cwd": "${workspaceFolder}",
            "binaryPath": "${workspaceFolder}/build/your_project.elf"
        }
    ]
}
```

## 4. (Опционально) Настройте задачи для сборки

Создайте `.vscode/tasks.json`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build",
            "type": "shell",
            "command": "make",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"]
        }
    ]
}
```

## 5. Проверьте работу Intellisense

- Откройте любой `.c` или `.h` файл.
- Проверьте, работает ли автодополнение, переход к определению, подсветка ошибок.

## 6. Советы

- Если Intellisense не видит какие-то файлы, проверьте `includePath` и `defines`.
- Для STM32 можно сгенерировать список include-путей и defines из Makefile командой:
  ```
  arm-none-eabi-gcc -MM -E -x c main.c
  ```
  и добавить их в `c_cpp_properties.json`.

---

**Пример структуры проекта:**
```
.
├── Core
│   ├── Inc
│   └── Src
├── Drivers
├── Makefile
└── .vscode
    ├── c_cpp_properties.json
    └── tasks.json
```

---

Если потребуется пример для конкретного семейства STM32 или помощь с генерацией include-путей — напишите, помогу!