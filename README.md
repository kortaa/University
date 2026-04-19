# uni

Collection of university C projects, each in its own directory.

## Projects

| Directory | What it is |
| --- | --- |
| `malloc/` | Custom dynamic memory allocator with driver-based benchmarking and grading scripts. |
| `shell21/` | Small Unix-like shell implementation with jobs, lexer/parser logic, and automated tests. |
| `slowomania/` | SDL2 typing game focused on typing speed and score tracking. |
| `tictactoe/` | Terminal Tic-Tac-Toe game against an AI using Minimax. |

## Repository Structure

This repository is intentionally split into independent projects.
Build, run, and test commands are executed inside each project directory.

## Prerequisites

- C compiler (GCC or Clang)
- GNU Make (required for `malloc/` and `shell21/`)
- Python 3 (used by grading/testing scripts)

Project-specific dependencies:

- `shell21/`: `readline` development package
	- Debian/Ubuntu: `sudo apt install libreadline-dev`
	- Fedora: `sudo dnf install readline-devel`
- `slowomania/`: SDL2, SDL2_image, SDL2_ttf (+ runtime assets in folder)

## Quick Start

### `malloc/`

Build:

```sh
cd malloc
make
```

Run driver:

```sh
./mdriver
```

Run grading script:

```sh
make grade
```

Clean build files:

```sh
make clean
```

### `shell21/`

Build:

```sh
cd shell21
make
```

Run shell:

```sh
./shell
```

Run tests:

```sh
make test
```

Notes:

- Test script runs multiple randomized checks (`sh-tests.py`).
- Build includes AddressSanitizer flags in current `Makefile`.

### `slowomania/`

This project ships SDL runtime DLLs/assets in `slowomania/`.
There is no Makefile in this directory right now.

Typical build on Windows with MinGW (adjust include/lib paths to your SDL setup):

```sh
gcc main.c -o slowomania -lSDL2 -lSDL2_image -lSDL2_ttf
```

Then run from inside `slowomania/` so assets like `words.txt`, `best.txt`, and images are found.

### `tictactoe/`

Build:

```sh
cd tictactoe
gcc main.c tic.c -o tictactoe
```

Run:

```sh
./tictactoe
```

## Platform Notes

- `malloc/` and `shell21/` are Unix/Linux-oriented by default.
- `tictactoe/` uses `system("cls")` in code, so it is currently Windows-console oriented.
- `slowomania/` is currently easiest to run on Windows because SDL DLLs are already included.

## Formatting

If `clang-format` is installed:

- `malloc/`: `make format`
- `shell21/`: `make format`

## Academic Integrity

This repository contains coursework-style implementations.
Use responsibly and follow your institution's collaboration/cheating policy.
