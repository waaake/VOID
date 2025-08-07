# Changelog

All notable changes to VOID player will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),  
and VOID player adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.1.0-alpha] - 2025-08-07
### Added
- Initial public release of VOID.
- Media playback support (EXR via OpenEXR, MOV/MP4 via FFmpeg, common image formats like png, tiff, dpx, jpg via OpenImageIO).
- Renderer with A/B buffer support and image comparison.
- Channel playback: R, G, B, A, RGB, RGBA, ALPHA.
- OpenColorIO integration for color management.
- Annotation tools: brush and text with multi color.
- Fullscreen playback and variable framerate support.
- Undo queue system.

### Misc
- Basic Docking system UI for docking component widgets as Tabs.
- Dock manager to hold widgets that can be docked via the Dock system.
