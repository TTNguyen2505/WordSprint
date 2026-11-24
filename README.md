# WordSprint

WordSprint is a desktop English vocabulary puzzle game. It aims to help learners practice vocabulary, listening, reaction speed, and typing through short, timed challenges.

> **Current status:** The menu, profiles, stage progress, and leaderboards load from SQLite through Qt WebChannel. Profile creation and audio settings are saved; profile deletion is disabled. Stage cards open a mock play screen with pause, resume, replay, and quit navigation. Its result controls preview navigation but do not save scores or unlock stages. Answer checking, the gameplay timer, audio playback, and other menu screens are not yet implemented.

## Product vision

Players solve vocabulary puzzles organized by topic and CEFR level (A1–C2). Each playthrough is intended to select a randomized set of words, and completing a stage unlocks the next one.

### Planned puzzle types

- **Scramble:** Rearrange shuffled letters to form a word, using an image as a hint.
- **Fill in the Blank:** Complete a word with missing letters using the available word and context clues.
- **Audio:** Listen to a pronunciation a limited number of times, then type the matching word.

The planned answer flow gives immediate visual feedback and advances to the next question after a correct answer. After an incorrect answer, the entered text is selected so the player can try again. Each stage has a countdown timer.

### Planned progress and player profiles

The in-game HUD is intended to show questions in **Solved**, **Current**, and **Queued** states, along with the score, remaining questions, and time. Players will be able to pause and resume a game.

The game is also intended to support multiple profiles on one computer, with separate progress and achievements; independent music and sound-effect controls; and stage and overall leaderboards.

### Visual direction

The planned **Neo-brutalism** visual style uses bold borders, sharp corners, hard shadows, and a high-contrast palette of lime yellow, neon green, bright red, white, and black.

## Technology

- **Desktop application:** C++20, Qt 6, QML, and Qt WebEngine.
- **Frontend:** React 19, Vite, and Tailwind CSS 4.
- **Frontend state and data tools:** Zustand and TanStack Query.
- **Build:** CMake builds the desktop application; Vite builds the frontend, which Qt can embed as application resources.

Qt/QML provide the desktop shell and display the React frontend in a `WebEngineView`. There is no separate backend service; the C++ repositories use a local SQLite database. Gameplay does not yet write progress from the frontend.

Seeded words use placeholder image paths under `storage/images/`; the image files are not included yet.
Files in `storage/` are embedded at build time under `qrc:/storage/`, preserving their subdirectories.
The WebChannel currently returns the database paths as relative strings, not `qrc:` URLs.
Audio paths are optional and point to `storage/audio/` when audio is available. On the first launch of
this version, the application resets an existing unversioned database, deleting its old profiles,
progress, and vocabulary. Later launches retain the new data.

## Clone the repository

```powershell
git clone https://github.com/TTNguyen2505/WordSprint.git
cd WordSprint
```

## Requirements

This project setup targets **Windows x64**.

| Tool | Requirement | Purpose |
| --- | --- | --- |
| Git | Current version | Clone and update the repository |
| Visual Studio 2026 | Workload **Desktop development with C++**, including MSVC (latest version) and a Windows SDK | C++ compiler and build tools |
| Windows SDK | Includes `winsqlite3.h` and `winsqlite3.lib` | SQL script parsing |
| CMake | 3.22 or newer | Configure and build the desktop application |
| Qt | Qt 6 for MSVC 2022 64-bit, with **Core**, **Gui**, **Qml**, **Quick**, **WebEngineQuick**, **WebEngineCore**, **Qt Positioning** and **WebChannel** | Desktop shell, QML, and embedded web view |
| Node.js | 22.12 or newer | Run the Vite frontend and install its tools |
| pnpm | Current version | Install frontend dependencies and run frontend scripts |

On non-Windows platforms, the backend requires the SQLite3 development library for SQL script parsing.

### Check the installed tools

Run these commands in PowerShell:

```powershell
git --version
cmake --version
node --version
npm --version
pnpm --version
```

In **Developer PowerShell for VS 2022**, check the C++ compiler:

```powershell
cl /Bv
```

To check Qt, use the `qmake.exe` path for your installed Qt kit. For the default path used by this project:

```powershell
& "C:\Qt\6.11.3\msvc2022_64\bin\qmake.exe" --version
```

If Qt is installed elsewhere, change the path to match your installation. CMake configuration also verifies that it can find Qt and all required modules.

### Install the development tools

1. **Visual Studio:** Install Visual Studio 2022 and select **Desktop development with C++** in Visual Studio Installer. Make sure the MSVC v143 x64 tools and a Windows SDK are included.
2. **Qt:** Use the Qt Online Installer to install a Qt 6 kit built for **MSVC 2022 64-bit**. Select the required Qt modules listed above, especially Qt WebEngine and Qt WebChannel. The Qt kit must use the same compiler and architecture as the project.
3. **CMake and Git:** Install them if they are not already available in your terminal. CMake is also available through Visual Studio Installer, but use version 3.22 or newer.
4. **Node.js and pnpm:** Install pnpm or newer, then install Node.js:

   ```powershell
   irm https://get.pnpm.io/ps1 | iex
   ```

   ```powershell
   Invoke-WebRequest https://pnpm.io -UseBasicParsing | Invoke-Expression
   ```

   Restart PowerShell after installation if `node`, or `pnpm` is not recognized.

The CMake project has a convenience fallback for Qt at `C:\Qt\6.11.3\msvc2022_64`. If your Qt kit is installed in another location or uses another version, set `CMAKE_PREFIX_PATH` to that kit's root when configuring the project.

Install the JavaScript dependencies before configuring CMake. If `frontend/dist/index.html` does not exist, CMake may try to build the frontend during configuration; that build requires the frontend dependencies to have already been installed.

## Install and run

### Run the frontend in a browser

From the repository root:

```powershell
cd frontend
pnpm install --frozen-lockfile
pnpm dev
```

Vite starts at `http://localhost:5173`. Other frontend commands, run from `frontend`:

```powershell
pnpm build
pnpm lint
pnpm format:check
```

### Build the Qt desktop application

First install the frontend dependencies and build the frontend. Then configure and build the desktop application from the repository root:

```powershell
cd frontend
pnpm install --frozen-lockfile
pnpm build
cd ..
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.11.3\msvc2022_64"
cmake --build build --config Release
```

Replace `C:\Qt\6.11.3\msvc2022_64` with the root directory of your Qt MSVC 2022 64-bit kit. On Windows, the build runs `windeployqt` after building the application to deploy the required Qt runtime files. The executable is created under `build\backend\Release`; the exact location may vary with the CMake generator and configuration.

### Develop the frontend with the desktop application

Start the Vite development server in one PowerShell window:

```powershell
cd frontend
pnpm dev
```

In another window, run the Qt application with `--debug`:

```powershell
<path-to-WordSprint.exe> --debug
```

The application connects to `http://localhost:5173` by default in debug mode. To load a different URL, use `--url`:

```powershell
<path-to-WordSprint.exe> --url "http://localhost:5173"
```

## Project structure

```text
.
├── backend/   # C++/Qt application and QML window hosting Qt WebEngine
├── frontend/  # React UI, Vite configuration, and dependencies
├── report/    # LaTeX report
└── english-vocabulary-puzzle-game.md  # Gameplay and product-vision document
```

## Documentation

- [Frontend](./frontend/)
- [Report](./report/)
