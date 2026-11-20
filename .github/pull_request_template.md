## Jira Tracking
- **Jira Task:** [OOP-XX](https://your-domain.atlassian.net/browse/OOP-XX)
- **Parent Epic:** [OOP-YY](https://your-domain.atlassian.net/browse/OOP-YY) <!-- Optional: remove if not applicable -->
- **Related GitHub Issue:** N/A <!-- Or Closes #123 if tracking GitHub issues in parallel -->

## Description
<!-- Briefly summarize what changes were made and why. -->


## Type of Change
<!-- Check [x] all applicable options -->
- [ ] `feat`: New feature (React UI, Qt desktop shell, or C++/SQLite functionality)
- [ ] `fix`: Bug fix in the React frontend, Qt/C++ application, or local data handling
- [ ] `refactor`: Code refactoring without changing behavior
- [ ] `perf`: Performance improvement (frontend rendering, Qt WebEngine, or SQLite)
- [ ] `docs`: Documentation updates (LaTeX report, README, or technical docs)
- [ ] `chore`: Build, tooling, assets, or formatting updates (pnpm, Vite, CMake, or Qt)
- [ ] `!` BREAKING CHANGE (changes that alter existing APIs or architecture)

## Visual Changes (React / QML)
<!-- Attach before/after screenshots or a short demo clip for React or QML changes. Write N/A for backend or logic-only tasks. -->
- *N/A*

## Validation
<!-- List the exact checks run and their results. Use N/A when a check is unrelated to this change. -->
- Frontend (`cd frontend`): `pnpm lint`, `pnpm format:check`, `pnpm build` — *N/A*
- Desktop app (repository root): `cmake --build build --config Release` — *N/A*
- C++/QML formatting (`clang-format` / `qmlformat`, when applicable) — *N/A*
- SQLite or gameplay checks (when applicable) — *N/A*

## Checklist
<!-- Check [x] all items before requesting a review -->
- [ ] **PR Title** follows the Jira convention: `[OOP-XX] <type>(<scope>): <description>` (e.g., `[OOP-13] feat(ui): build design system tokens`)
- [ ] Rebased onto the latest `main` branch prior to pushing
- [ ] React/JavaScript and CSS changes pass the applicable pnpm lint and format checks
- [ ] C++ and QML changes follow the applicable `clang-format` and `qmlformat` conventions
- [ ] Applicable frontend and/or Qt desktop builds were run and their results are recorded above
- [ ] No new compiler warnings, browser console errors, QML warnings, or SQLite/data-integrity issues introduced
