# Tetris Clock

Qt6/QML implementation of a Tetris-style clock — Tetris pieces fall to form the current time (HH:MM), inspired by [WiFi-Tetris-Clock](https://github.com/witnessmenow/WiFi-Tetris-Clock) and the [TetrisAnimation](https://github.com/toblum/TetrisAnimation) library.

Runs on **Windows / macOS / Linux** and **Android** (tablet-friendly, landscape).

![Tetris Clock screenshot](docs/screenshot.png)

## Features

- Authentic Tetris piece animations (piece shapes and rotations match the original Arduino library)
- 24h / 12h display toggle
- 5 color themes: Classic, Monochrome, Pastel, Neon, Retro
- Grid dot color picker
- Brightness and animation speed sliders
- Fullscreen mode
- Click to replay animation; long-press or right-click to peek at the date (MM/dd)
- All settings saved between sessions (QSettings)

## Build

### Requirements

- Qt 6.5 or later (Quick, Core)
- CMake 3.21+

### Desktop

```bash
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/<platform>
cmake --build build
```

### Android

1. Install **Qt for Android ARM64-v8a** via Qt Maintenance Tool
2. Install **Android SDK** (API 36), **NDK r27**, **JDK 17**
3. Configure the Android kit in Qt Creator
4. For release signing, create `android-signing.properties` (see below) — **do not commit this file**

```properties
storeFile=/path/to/your.jks
storePassword=yourpassword
keyAlias=youralias
keyPassword=yourpassword
```

Set the environment variable `ANDROID_SIGNING_PROPERTIES` to the absolute path of this file in Qt Creator's Build Environment, or place it two directories above the Gradle build directory.

## Credits

Digit and piece definitions ported from [toblum/TetrisAnimation](https://github.com/toblum/TetrisAnimation) (LGPL 2.1).
