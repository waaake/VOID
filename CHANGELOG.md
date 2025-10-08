# Changelog

All notable changes to VOID player will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),  
and VOID player adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.2.0-alpha] - 2025-10-08
### Added
- Projects are supported in VOID player.
- Projects can be created/saved and loaded back.
- Component serialization and deserialization support through VoidObject with support for ASCII (ether) and Binary (nether) formats.
- Support for Media directories to be imported through UI menu or dragging the directory over the Media View.
- Support for playlists within a Project where media can be organized for quick playback and review.
- Timeline media caching mechanism.
- Support for dragging multiple media directly on to the Viewer which will play them as a sequence or a single media.
- Image Metadata viewer for viewing basic metadata from the various image types.
- Support for scripting in VOID script editor.
- VOID python bindings 'vortex'.
- Material icon font based icons support for the player to allow customization of icon color when switching themes.
- Multiple color scheme support.
- Internal Menu system handling Menus in the VOID Menu bar.
- Extension of Menu system to the python API allowing Menu items to be created from python.

### Fixed
- Minor Memory leaks due to Qt objects not initialized with proper parents causing the clean up to ignore when parent is destroyed.
- Dock panel menu pops-up instantly as opposed to a small wait time earlier when the button reverts to its original state.
- Issue with internal FFmpeg reader where seeking was not working as expected when jumping between frame randomly on Movie type media.

### Misc
- Internal organisation of the project components to make Undo a UI entity.
- Optimized Movie file import times by removing additional queries to 'avformat_find_stream_info' when reading movie frames and framerate using FFmpeg API.
- Added Project Bridge to deal with Project components.


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
