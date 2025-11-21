# Docking and Theming Roadmap

This document captures the phased plan for hardening the docking system, centralizing theming, adding a dock/taskbar-style "dock bar," and scaffolding a custom theme builder. Each item includes actionable task stubs.

---

## Phase 1: Centralize and activate theming (remove ad-hoc styling)
Goal: Apply a single theme at app startup, eliminate widget-local hard-coded styles, and make palette tokens reusable across docks.

1. **Apply global theme at launch**
   - Ensure `QApplication` (or `MainWindowGPU` bootstrap) sets the active theme via `ThemeManager` so the stylesheet is applied globally instead of per-widget tweaks.
   - Provide a theme toggle entry point (menu/toolbar) that calls into `ThemeManager`.

   :::task-stub{title="Wire ThemeManager into app startup and toggle UI"}
   - Locate the main entry (likely `apps/sentinel_gui` or `MainWindowGPU`) and call `ThemeManager::instance().applyTheme(defaultTheme)` right after `QApplication` creation.
   - Add a menu/toolbar action to switch themes at runtime; when invoked, re-apply the chosen theme globally and persist the choice in settings.
   - Confirm that the default theme is loaded on next startup by reading settings before showing the main window.
   :::

2. **Refactor hard-coded styles to theme tokens**
   - `HeatmapDock` and the status bar have inline styles (colors/typography). Replace them with theme palette/spacing tokens exposed by `ThemeManager`.

   :::task-stub{title="Replace inline styles in docks/status bar with theme tokens"}
   - In `HeatmapDock` (or equivalent), remove widget-local `setStyleSheet` definitions for headers/symbol bars; instead, expose semantic tokens (e.g., `panelBg`, `accent`, `textMuted`, `radiusSm`) in `ThemeManager`.
   - In `MainWindowGPU` (or main window class), replace status-bar inline styles with themed classes or stylesheet selectors driven by the global theme.
   - Add/adjust theme QSS to cover `.StatusBar`, dock headers, and toolbar buttons so docks inherit consistent styling.
   :::

3. **Document theme tokens and usage**
   - Provide a short guide for contributors to add/extend themes without reintroducing inline styles.

   :::task-stub{title="Author theme usage guide"}
   - Create/update `docs/` (e.g., `docs/theming.md`) describing available tokens, how to add a new theme class, and how to bind widgets to semantic selectors.
   - Include a note on avoiding per-widget styles and preferring theme selectors.
   :::

---

## Phase 2: Harden docking system behavior and persistence
Goal: Reliable layout save/restore, clean handling of incompatible layouts, and clearer defaults.

1. **Separate “factory default” from “last session”**
   - Prevent the default layout from being overwritten on close; store last session separately.

   :::task-stub{title="Split layout presets: factory vs last-session"}
   - In dock persistence code (likely around `QSettings` usage), define distinct keys for `factory_default` and `last_session`.
   - Load order: `last_session` (if compatible) -> `factory_default` fallback -> build-time layout.
   - On close, only write `last_session`; preserve `factory_default`.
   :::

2. **Handle incompatible/failed restores gracefully**
   - Auto-prune bad layouts and avoid repeat warnings.

   :::task-stub{title="Auto-purge incompatible dock layouts on restore failure"}
   - When restore fails (version mismatch or deserialization error), delete the offending entry in `QSettings` and notify the user once.
   - Add a minimal version stamp to saved layouts; compare before restore to detect incompatibility early.
   :::

3. **Lifecycle and docking UX consistency**
   - Verify dock events (detach/attach, close/minimize) emit consistent signals; ensure focus/selection behavior is predictable.

   :::task-stub{title="Audit dock lifecycle signals and focus handling"}
   - Inspect dock manager classes for signals on detach/attach/close; ensure they fire once and update menus/toolbars accordingly.
   - Standardize focus behavior when a dock is restored or shown (e.g., raise + focus the main widget).
   - Add a small regression checklist to `docs/` for docking interactions (detach, tab reorder, restore defaults).
   :::

---

## Phase 3: Add a dock/taskbar-style “dock bar”
Goal: Provide a taskbar-like area where minimized/hidden docks live, enabling quick restore/launch.

1. **Create dock bar UI**
   - A horizontal strip (like Windows/macOS dock/taskbar) that lists available/active docks with icons and states.

   :::task-stub{title="Implement dock bar widget and integration"}
   - Add a `DockBar` widget (e.g., under `libs/gui/docking/`) that shows dock entries with icon, title, and state (active/minimized).
   - Wire dock manager events to populate/refresh the dock bar when docks are added, detached, minimized, or closed.
   - Support click-to-restore and context menu actions (close, detach, reset).
   :::

2. **Minimize-to-dock-bar behavior**
   - Allow docks to be minimized into the dock bar instead of closing or hiding in menus.

   :::task-stub{title="Add minimize-to-dock-bar flow"}
   - Add a “minimize” command to dock titlebars/context menus that hides the dock content and registers it in the dock bar.
   - Ensure restore brings the dock back to its previous position (tab/float) using the persistence layer.
   :::

3. **Persist dock bar state with layouts**
   - Store which docks are minimized/visible in the saved layout.

   :::task-stub{title="Persist dock bar entries in layout save/restore"}
   - Extend layout serialization to include minimized state and dock bar ordering.
   - On restore, re-create dock bar entries before showing the main window to avoid flicker.
   :::

---

## Phase 4: Theme selector and “build your own theme” starter
Goal: Runtime theme switching UI plus scaffolding for a custom theme builder.

1. **Theme selector panel**
   - A simple dock/panel listing available themes with preview chips.

   :::task-stub{title="Add theme selector dock/panel"}
   - Create a `ThemeSelectorDock` that enumerates registered themes from `ThemeManager`.
   - Provide preview swatches (primary/bg/text) and apply-on-click; persist the chosen theme.
   - Integrate with the dock bar and layout persistence so it can be minimized/restored like other docks.
   :::

2. **Starter “build your own theme” widget**
   - Basic form to define colors/spacing and generate a theme object/QSS.

   :::task-stub{title="Scaffold custom theme builder UI"}
   - Add a new dock/panel (e.g., `CustomThemeBuilderDock`) with inputs for key tokens (primary, background, text, accent, radius/spacing).
   - Provide live preview using a small sample widget set; hook a “Save as new theme” action that registers the theme with `ThemeManager` for the current session.
   - Persist user-created themes to a simple file (JSON/YAML) and load them at startup.
   :::

3. **Validation and safety**
   - Ensure invalid palettes don’t break the UI.

   :::task-stub{title="Add theme validation and safe fallbacks"}
   - Validate contrast/required tokens; if invalid, fall back to the last good theme and show an error message.
   - Keep a “Reset to default” control in the selector/builder panels.
   :::

---

## Phase 5: Documentation and guardrails
Goal: Keep future contributions aligned with centralized theming and dock behavior.

1. **Docs for docking + dock bar**
   - Extend existing docking docs with the new dock bar, minimize flow, and persistence rules.

   :::task-stub{title="Update docking docs with dock bar and persistence rules"}
   - Document dock bar UX, minimize/restore, and how layouts now track minimized state and versioning.
   - Include a short troubleshooting section (e.g., clearing layouts) and a regression checklist.
   :::

2. **Theming contribution guide**
   - Brief section on adding themes and using tokens, plus notes on avoiding inline styles.

   :::task-stub{title="Extend theming docs with contribution rules"}
   - Add do/don’t examples for styles (prefer theme selectors; avoid per-widget QSS).
   - Describe how to register themes, add tokens, and test with the selector/builder.
   :::

---

*This plan keeps the theme system centralized, strengthens docking reliability, introduces a dock/taskbar metaphor, and scaffolds a custom theme builder to encourage contributor-friendly theming.*