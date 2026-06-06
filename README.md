# hud-tryer

Minimal LeviLamina plugin for testing Bedrock HUD layers:

- `actionbar`
- `subtitle`
- `title`

## Commands

- `/hudtry actionbar`
- `/hudtry subtitle`
- `/hudtry title`
- `/hudtry all`
- `/hudtry clear`
- `/hudtry reset`

All commands are player-only. They send intentionally similar multiline content so you can compare:

- position
- wrapping
- background
- lifetime
- overlap behavior

## Build

This repository already includes GitHub Actions workflows for build and release.

Local build:

1. `xmake repo -u`
2. `xmake f -a x64 -m release -p windows --target_type=server -y`
3. `xmake`
