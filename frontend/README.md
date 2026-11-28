# WordSprint Frontend

This directory contains WordSprint's React frontend. It can be installed, run, checked, and built independently of the Qt desktop application. The same frontend is also used by the root project, where Qt WebEngine displays it inside the desktop shell.

> **Current status:** The menu, profiles, audio settings, leaderboards, and stage questions load through the Qt backend and require the desktop application. Profile creation and audio settings persist in SQLite; profile deletion is disabled. Audio questions use browser speech synthesis with an embedded Qt audio resource as a fallback, hide the hint image, and allow CEFR-based manual replays. Gameplay still does not check answers or save results.

## Design system

Shared CSS tokens and typography utilities are defined in [`src/index.css`](src/index.css). They cover the palette, semantic colors, spacing, icon sizes, stroke widths, elevations, blur, and the existing frontend text styles. Use these utilities instead of repeating a literal value when it matches a token, for example `bg-surface-primary-cta`, `text-body-default`, `gap-md`, `border-stroke-default`, and `shadow-elevation-200`.

Reusable UI components are in [`src/components/`](src/components/), including buttons, badges, tabs, inputs, modals, cards, and quiz elements. Keep component styles bound to the shared tokens when their existing values match exactly; retain one-off values that do not have a corresponding token.

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
The application needs the Qt WebChannel to load its data, so run it in the desktop app for the menu and profile workflows. A standalone browser reports that the backend is unavailable.

## Build and check

Run these commands from `frontend/`:

| Command                                         | Purpose                                                      |
| ----------------------------------------------- | ------------------------------------------------------------ |
| `pnpm build`                                    | Create the production frontend in `dist/`                    |
| `pnpm preview`                                  | Preview the production build locally; run `pnpm build` first |
| `pnpm lint`                                     | Run ESLint                                                   |
| `node --test src\viewModels\viewModels.test.js` | Run the focused view-model tests                             |
| `pnpm format:check`                             | Check formatting with Prettier                               |
| `pnpm format`                                   | Format files with Prettier                                   |

`dist/` is generated output and is excluded from version control.

## Use the frontend in the desktop app

The root project builds the C++/Qt application in `backend/`. Its QML window hosts this frontend in a Qt `WebEngineView`:

- **Development:** Start `pnpm dev`, then run the Qt application with `--debug`. The desktop app loads the Vite server at `http://localhost:5173`.
- **Production:** Run `pnpm build` before configuring/building the root CMake project. The Qt build includes the generated `dist/` files as application resources.

The root CMake configuration can invoke the frontend build when `dist/index.html` is missing, but dependencies must already be installed in this directory. Building `dist/` explicitly first makes the integration step predictable.

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
