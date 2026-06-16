# Space Shooter (Аркада в консоли)

Консольная игра: управляй кораблём, стреляй по врагам, побеждай боссов.

## Управление

- `A` / `D` (стрелки) — влево / вправо
- `Пробел` — выстрел

---

## Как собрать и запустить

### Windows/macOS

```
cd arcade_in_console
mkdir build
cd build
cmake ..
cmake --build .
./arcade_in_console.exe
```

---

## Запуск тестов

Из папки `build` после сборки:

```
ctest
```
