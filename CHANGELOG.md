# Changelog

All notable changes to VOID player will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),  
and VOID player adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.5.0-alpha] - 2026-11-04
### Added
- Added initial version of Sequencer which is an NLE, providing a visual representation of Sequence and Items within it.
- Sequencer supports tagging Tracks and TrackItems.
- Effects can be added on TrackItems which gets processed before the frame is rendered on screen.
- Added support for snapshot of a sequence which saves the current state of the sequence in time for restoring back later if required.
- Sequencer supports dragging media from the Media View to the tracks within the sequence.
- Added Spreadsheet widget allowing media to be set as Reference media on the active sequence.
- Media can also be relinked using Default and Custom Conform rules and support is extended to be created from python.
- Added support for media versioning based on the naming pattern of the media, having (v/V followed by numbers e.g. v001) is treated as the version of the element.
- Sequencer allows finding and importing any parallel versions to the project present in the root directory of media.
- Media having multiple versions can be switched in the sequence allowing users to have the latest elements in the timeline always.
- Added support for showing timecodes in the Timeline and Sequencer with the drop down in the timeline.

### Fixed
- Fixed slowness during playback due to drawing too many elements in the Timeslider when the frame range is huge, by limiting the amount of ticks that gets drawn for any range.

### Misc
- Restructured internal components to be separate libraries within VOID.
- Improved the media drag import to support multiple directories at once.
- Imporved adding multiple media to the track on a sequence which now is much faster than before.
- Improved performance in reading Media through JPEG, EXR and FFmpeg Media readers.
- Improved audio playback support in the player across all platforms.
- Audio can be played through the sequence timeline as well.
- Track items are searched and indexed through binary search as they are sorted all the time, improving performance where used.
- Executed python script in the script editor widget retains after reopening VOID.
- Improved undo-redo for media items which now serialises and deserialised the media compared to copying internal information before.
- Ensures that the code is now compatible with Qt 6.8 as well.
- Updated playlist view to be similar to the Media view in terms of interface.

## [0.4.0-alpha] - 2026-07-04
### Added
- Added support for Grid/Contact Sheet view of images, which can also be used for QC in a layout.
- Initial version of image processing, with processing done through ImageOperators that can be extended externally to VOID. Example to follow soon.
- Image Operator properties is presented to the UI through the Properties panel on the Effect Editor.
- Added Custom Tags, allowing media to be tagged with them and some metadata.
- Tag metadata is also shown in the Metadata Viewer under the key Tags/${tagname}/${keyname}
- Added application argument to allow setting the default framerate when the application launches.
- Added support for scaling the UI, and can be done with the --scale n argument for the application.
- Added Media Exporter which transcodes the current media into the specified outformat.
- Added support for exporting current and Annotated frames as .png media, separate options to select outformat is coming in the next release.
- Media export is handled in a separate thread and is managed by the Task Queue.

### Fixed
- Fixed an issue where player was crashing when switching from Image media to Movie Media and vice-versa.
- Fixed an issue where the Image metadata was not being displayed for the correct media when dragging and dropping, this was causing crash when the first item was dropped.

### Misc
- Allow toggling media channels on the viewer with 'r', 'g', 'b' and 'a' keyboard keys for shortcut.
- Extended Undo and Redo functionality for Custom Tags and Effects applied on the Media.
- Optimise FFmpeg decoder to directly give the next frame and not flood the memory with additional frames.
- FFmpegDecoder supports 16 parallel decoded instances to support comparing and Grid/Layout buffers.
- PixReader now forces child readers to use float* buffer and output Linear data, this ensures that all operations performed on the data are identical across media types.
- Improved threaded media importer to use std::async instead of QThread based worker which was causing crashes some times due to improper exit.
- Added a View to edit/control the items in the Queue/Grid.
- Blend modes in the viewer can now be cycled through the menu options and also keyboard shortcuts.
- Media list now shows the channels of the media (basic implementation only).
- Thumbnail view in the media list now supports scaling the size of the Thumbnail/delegate.


## [0.3.0-alpha] - 2026-03-12
### Added
- Support for dragging and dropping docked panels.
- Initial concept of Player workspaces allowing the users to switch between Basic, Standard Playback, Review with playlist and scripting workspaces.
- Support for Audio playback on Linux and Windows. Mac audio support is partly added (some bugs need to be fixed and overall experience improvements needed.)
- Recent projects startup window, allowing the users to quickly choose from the last project(s) (upto 5) being worked on.
- Custom media importer with support for Sequence representation (image_name.####.ext 1001-1120) format for image sequences when being browsed through the file browser for VOID.
- Support for command-line arguments like --media, --project to load a media or a project directly when opening the player.
- Support for switching to the basic workspace view with --basic command-line and can be used alongside --media for a quick view over the media.
- Support for playing multiple media in a queue which makes them play one after the other and keeps the timeline range respective to the current media being played.

### Fixed
- Fixed an issue where the frameless titlebar allowed the player to move even when it is in maximized state.
- Fixed the player to clear the current media before accepting the dropped media.

### Misc
- Minor UI elements overhaul for the playback controls and timeline to make the buttons more asthetic and improve usability/accessibility.
- Icons appear with slightly better contrast with the backgrounds especially the dark themes.
- Improved color schemes for the player and made the overall theme contrast better.
- In project view, the saved projects can be easily identified with the sidebar color accent.
- Imported media thumbnails are generated on thread, keeping the load process faster and smooth for improved user experience.
- Playlists created within the project are also saved and loaded with the project.
- Switched to OpenGL Pixel Buffer Objects for improved rendering and playback rate.
- Improvements to how the internal FFmpegDecoder provides Data to the PixelReader.
- Improvements to the Media entity construction for slight benefits in terms of performance.
- Internal improvements to the Player architecture for better distinction of widget v/s the core logic.


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
