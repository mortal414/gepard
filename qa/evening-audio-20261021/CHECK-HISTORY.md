# Check history

- Initial source/static and stock-index checks passed.
- The first staged formatting check detected CRLF endings in generated JSON because this repository preserves byte line endings (`* -text`). The first local checkpoint retained that defect; a follow-up changes the generators to explicit UTF-8/LF and regenerates both JSON reports. No production file changed.
- One repair command piped Python output into `Select-Object -First`, which interrupted stdout and yielded a nonzero process status. That shell attempt was not counted as a successful verification. Capturing complete process output before shortening its display fixed the runner; both Python checks then exited 0, and staged `git diff --check` passed.
- Native compilation, resource resolution, authoring, gameplay and listening were never attempted in this lane. They remain pending the integration owner's admission and supported graph authoring, not denied and not uncertain.
