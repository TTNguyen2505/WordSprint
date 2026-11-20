# WordSprint Frontend

This directory contains WordSprint's React frontend. It can be installed, run, checked, and built independently of the Qt desktop application. The same frontend is also used by the root project, where Qt WebEngine displays it inside the desktop shell.

> **Current status:** The frontend still shows the React/Vite starter screen. The vocabulary puzzles and gameplay described in the [product overview](../english-vocabulary-puzzle-game.md) are planned features, not completed functionality.

## Requirements

- Node.js 22.12 or newer.
- pnpm 9 or newer (the project uses `pnpm-lock.yaml`).

Qt, CMake, and a C++ compiler are **not** required to work on or build the frontend by itself.

Check that Node.js and pnpm are available:

```powershell
node --version
pnpm --version
```

## Clone and install

From a directory where you want the repository:

```powershell
git clone https://github.com/TTNguyen2505/WordSprint.git
cd WordSprint\frontend
pnpm install --frozen-lockfile
```

If you have already cloned the repository, run `cd frontend` from the repository root, then install dependencies with `pnpm install --frozen-lockfile`.

## Develop in a browser

Start the Vite development server from this directory:

```powershell
pnpm dev
```

Open `http://localhost:5173` in a browser. The server uses port 5173 and fails to start if that port is already in use.

## Build and check

Run these commands from `frontend/`:

| Command             | Purpose                                                      |
| ------------------- | ------------------------------------------------------------ |
| `pnpm build`        | Create the production frontend in `dist/`                    |
| `pnpm preview`      | Preview the production build locally; run `pnpm build` first |
| `pnpm lint`         | Run ESLint                                                   |
| `pnpm format:check` | Check formatting with Prettier                               |
| `pnpm format`       | Format files with Prettier                                   |

`dist/` is generated output and is excluded from version control.

## Use the frontend in the desktop app

The root project builds the C++/Qt application in `backend/`. Its QML window hosts this frontend in a Qt `WebEngineView`:

- **Development:** Start `pnpm dev`, then run the Qt application with `--debug`. The desktop app loads the Vite server at `http://localhost:5173`.
- **Production:** Run `pnpm build` before configuring/building the root CMake project. The Qt build includes the generated `dist/` files as application resources.

The root CMake configuration can invoke the frontend build when `dist/index.html` is missing, but dependencies must already be installed in this directory. Building `dist/` explicitly first makes the integration step predictable.

At startup, the frontend shows a splash screen while it loads the bundled fonts, verifies the frontend assets, and asks the Qt desktop backend to open and check its SQLite database. Running the frontend directly in a browser skips SQLite and displays a notice on the menu because a browser preview cannot access the desktop database.

The frontend can also be built without Qt:

```powershell
pnpm install --frozen-lockfile
pnpm build
```

For complete desktop prerequisites and CMake instructions, see the [root README](../README.md). The academic report is a separate LaTeX document and is not part of the frontend build; see the [report README](../report/README.md).

## Frontend structure

```text
frontend/
├── public/       # Static assets served/copied by Vite
├── src/          # React application source and styles
├── index.html    # Vite entry page
├── package.json  # Dependencies and scripts
└── vite.config.js
```

Vite is configured with a relative asset base (`./`) so the production bundle can load from the Qt application's embedded resources as well as from a web server.
