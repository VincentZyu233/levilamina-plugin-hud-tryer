<div align=center>

[![Downloads](https://img.shields.io/github/downloads/__REPO__/__VERSION__/total?style=flat-square&logo=github)](https://github.com/__REPO__/releases/tag/__VERSION__)
[![LeviLamina](https://img.shields.io/badge/for-LeviLamina-7FA973?style=flat-square&logo=cplusplus&logoColor=white&labelColor=2C5E3B)](https://github.com/LiteLDev/LeviLamina)

</div>

### ⬇️ Downloads

| File | Description |
|------|-------------|
| [📦 `hud-tryer-server-windows-x64.zip`](__BASE_URL__/hud-tryer-server-windows-x64.zip) | Plugin package for LeviLamina |

### 📥 Install via lip

```bash
lip install github.com/__REPO__@__VERSION__
# If latest-version resolution works in your environment, you may also try:
lip install github.com/__REPO__
# To update an already installed package:
lip update github.com/__REPO__@__VERSION__
```

### 📦 Manual Install

1. Download `hud-tryer-server-windows-x64.zip`
2. Unzip, you'll get:
   ```
   hud-tryer/
   ├── manifest.json
   ├── hud-tryer.dll
   └── hud-tryer.pdb
   ```
3. Copy `hud-tryer/` into your server's `plugins/`:
   ```
   BDS Server Root/
   ├── bedrock_server_mod.exe
   ├── PreLoader.dll
   ├── plugins/
   │   ├── LeviLamina/
   │   │   ├── LeviLamina.dll
   │   │   └── ...
   │   └── hud-tryer/
   │       ├── manifest.json
   │       ├── hud-tryer.dll
   │       └── hud-tryer.pdb
   └── ...
   ```
4. Restart the server

---

### 📋 What's Changed

__COMMIT_LOG__

---

### 📊 Build Info

- **Build date**: __BUILD_DATE__
- **Commit**: __COMMIT_HASH__
- **Full Changelog**: __CHANGELOG_URL__
