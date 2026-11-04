# [<img src="../images/VOID_Logo_Full.svg" width="545"/>](../images/VoidPlayerBasic.png)

### A quick overview of VOID's interface and capabilities.

## Project & Media Organization

- Multi project support for Media
- Drag-and-drop support for directories
- Automatically builds structured playlists from media folders
- In-project multi playlist support for further grouping media for quick review & playback

[<img src="../images/VoidPlaylist.png" width="1000"/>](../images/VoidPlaylist.png)

---

## Media Playback

Supports a wide range of media formats:

- **Images**: PNG, EXR, JPG, TIFF, DPX, and couple more via **OpenImageIO**
- **Videos**: MOV and MP4—enabled via **FFmpeg** with audio play support.

The player also supports **media reader plugins**, allowing developers to extend format support by writing custom plugins.
 *An example plugin is available [here](https://github.com/waaake/VOID-Exr-plugin)*

[<img src="../images/VoidPlayerPlaying.png" width="1000"/>](../images/VoidPlayerPlaying.png)

---

## Playback Features

- Standard playback controls: Play, Pause, Seek, etc.
- **Dual-buffer playback**: A/B buffers for simultaneous media buffering and comparing.

[<img src="../images/VoidMultiBuffer.png" width="1000"/>](../images/VoidMultiBuffer.png)

[<img src="../images/VoidChannelSupport.png" width="1000"/>](../images/VoidChannelSupport.png)

---

## Comparison Tools

Dual-buffer mode enables rich media comparison using multiple viewer layouts:

- **Horizontal & Vertical Split**
- **Swipe Compare**
- **Stack Compare**

> *Blend modes are currently under development.*

[<img src="../images/VoidWipeCompare.png" width="1000"/>](../images/VoidWipeCompare.png)

[<img src="../images/VoidHorizontalCompare.png" width="1000"/>](../images/VoidHorizontalCompare.png)

---

## Annotation Support

- Add annotations directly on media for review and feedback.
- Useful for review workflows and visual notes.

> *Annotated frames can be exported as .png or .exr file format. Raw annotation data import/export is not currently implemented.*

[<img src="../images/VoidAnnotations.png" width="1000"/>](../images/VoidAnnotations.png)

---

## Sequence Viewer (Sequencer)

- Visualise the elements in an NLE supporting multiple tracks (Audio and Video).
- Support for Image Operators (ImageOp) to be applied directly on the timeline elements. 

> *Timeline Effects feature is currently WIP with only Grade and Invert node available.*

[<img src="../images/VoidSequencer.png" width="1000"/>](../images/VoidSequencer.png)

[<img src="../images/VoidGradeEffect.png" width="1000"/>](../images/VoidGradeEffect.png)

---

## Configuration Options

Customize player behavior with basic preferences:

- Default media view interface
- Customization of the color scheme
- Handling of missing frames
- Undo history settings

[<img src="../images/VoidPreferences.png" width="1000"/>](../images/VoidPreferences.png)

### Color Styles

| [<img src="../images/VoidTheme-1.png" width="500"/>](../images/VoidTheme-1.png) | [<img src="../images/VoidTheme-2.png" width="500"/>](../images/VoidTheme-2.png) |
|---------------------------------------------------------------------------------|---------------------------------------------------------------------------------|
| [<img src="../images/VoidTheme-3.png" width="500"/>](../images/VoidTheme-3.png) | [<img src="../images/VoidTheme-4.png" width="500"/>](../images/VoidTheme-4.png) |


## Scripting

Support for scripting on the player using python bindings.

VOID provides internal API via **vortex** python binding, allowing interaction with internal components of the system

[<img src="../images/VoidScriptEditor.png" width="1000"/>](../images/VoidScriptEditor.png)

---

## Credits & Licensing Notice

Screenshots used in this project feature content from the open movies **Big Buck Bunny**, **Wing It**, and **Sprite Fright**, created by the Blender Foundation.

These movies are made available under their respective Creative Commons licenses. Please refer to the original sources for licensing and attribution details.

> © Blender Foundation
