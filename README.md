# media_finder

## Как собрать?
```bash
cmake -B build
cmake --build build
```
---

## Запуск
```bash
./build/media_finder [директория] [интервал_секунд]
```

- `директория` — путь для сканирования. По умолчанию: `$HOME`.
- `интервал_секунд` — положительное целое число, период между сканированиями. По умолчанию: `10`.

Также поддерживается `-h`/`--help`.

Остановка по `Ctrl+C` (`SIGINT`) или `SIGTERM`.

---

## API

HTTP-сервер слушает `127.0.0.1:1234`.

```bash
curl http://localhost:1234/media_files
```

Ответ (`application/json`):
```json
{
    "audio":  ["/path/to/song.mp3"],
    "video":  ["/path/to/movie.mp4"],
    "images": ["/path/to/photo.jpg"]
}
```

Возвращается последний завершённый снапшот сканирования — запросы не блокируются обходом файловой системы.

---
## Архитектура  

Для работы с http используется библиотека httplib. При запуске программы создается сервер который слушает порт 1234 и принимает GET запросы по пути `/media_files`
в ответе возврашает JSON с тремя массивами путей к найденным медиафайлам: `audio`, `video`, `images`.
Поиск файлов происводится с помощью `recursive_directory_iterator`. После первого сканирования создается фоновый поток, который через `std::condition_variable::wait_for` спит `interval` секунд, выполняет повторный обход и атомарно подменяет снапшот.


## Поддерживаемые расширения
- **audio:** `.mp3 .wav .flac .aac .m4a .opus .wma .aiff .ape .dsd .amr .webm`
- **video:** `.mp4 .avi .mkv .mov .wmv .flv .webm .m4v .3gp .ts .mts .m2ts .ogv .vp8 .vp9 .av1`
- **images:** `.png .bmp .jpg .jpeg .gif .tiff .tif .webp .heic .heif .avif .svg`

Поддерживаемые расширения указаны в файле Scanner.hpp

```c++
    std::unordered_set<std::string> audioExts_ = {
        ".mp3", ".wav", ".flac", ".aac",
        ".m4a",".opus", ".wma", ".aiff",
        ".ape",".dsd",".amr", ".webm"
    };
    std::unordered_set<std::string> imageExts_ = {
        ".png", ".bmp",".jpg", ".jpeg",
        ".gif",".tiff", ".tif", ".webp",
        ".heic",".heif",".avif", ".svg",
    };
    std::unordered_set<std::string> videoExts_ = {
        ".mp4", ".avi",".mkv",".mov",
        ".wmv", ".flv",".webm",".m4v",
        ".3gp",".ts",".mts", ".m2ts",
        ".ogv",".vp8", ".vp9", ".av1"
    };

```

---

## Зависимости
- nlohmann/json — https://github.com/nlohmann/json/
- cpp-httplib — https://github.com/yhirose/cpp-httplib/
