// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

/* Pybind11 */
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

/* Internal */
#include "Definition.h"
#include "Operator.h"
#include "VoidCore/Media/Filesystem.h"
#include "VoidCore/VoidTools.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Effects/Effects.h"
#include "VoidObjects/Sequence/Sequence.h"
#include "VoidObjects/Sequence/Track.h"
#include "VoidObjects/Sequence/TrackItem.h"

VOID_NAMESPACE_OPEN

namespace py = pybind11;

namespace bindings {

void BindCore(py::module_& m)
{
    m.doc() = "Void Core Module.";

    // m.def("test", []()
    // {
    //     VOID_LOG_INFO("test");
    // });

    /* Media Type */
    py::enum_<MediaType>(m, "MediaType")
        .value("Image", MediaType::Image)
        .value("Movie", MediaType::Movie)
        .value("Audio", MediaType::Audio)
        .value("NonMedia", MediaType::NonMedia)
        .export_values();

    /* MEntry */
    py::class_<MEntry>(m, "MEntry")
        .def(py::init<const std::string&>(), py::arg("path"))
        .def("fullpath", &MEntry::Fullpath)
        .def("basepath", &MEntry::Basepath)
        .def("name", &MEntry::Name)
        .def("extension", &MEntry::Extension)
        .def("framenumber", &MEntry::Framenumber)
        .def("is_single_file", &MEntry::SingleFile)
        .def("is_valid", &MEntry::Valid);

    /* MediaStruct */
    py::class_<MediaStruct>(m, "MediaStruct")
        .def(py::init<const MEntry&, const MediaType&>(), py::arg("media_entry"), py::arg("media_type"))
    
        .def(py::init<const std::string&, const std::string&, const std::string&, v_frame_t, v_frame_t, unsigned int>(), 
                py::arg("basepath"), py::arg("name"), py::arg("extension"), py::arg("startframe"), py::arg("endframe"), py::arg("frame_padding"))

        .def_static("from_file", &MediaStruct::FromFile, py::arg("filepath"))
        .def("add", &MediaStruct::Add, py::arg("media_entry"))
        .def("validate", &MediaStruct::Validate, py::arg("media_entry"))
        .def("name", &MediaStruct::Name)
        .def("extension", &MediaStruct::Extension)
        .def("basepath", &MediaStruct::Basepath)
        .def("is_single_file", &MediaStruct::SingleFile)
        .def("is_empty", &MediaStruct::Empty)
        .def("is_valid", &MediaStruct::ValidMedia)
        .def("media_type", &MediaStruct::Type);

    /* Media */
    py::class_<Media>(m, "Media")
        .def(py::init<const MediaStruct&>(), py::arg("media_struct"))
        .def(py::init<const std::string&, const std::string&, const std::string&, v_frame_t, v_frame_t, unsigned int>(),
                py::arg("basepath"), py::arg("name"), py::arg("extension"), py::arg("startframe"), py::arg("endframe"), py::arg("frame_padding"));

    /* MediaClip */
    py::class_<MediaClip, SharedMediaClip>(m, "MediaClip")
        .def(py::init<const MediaStruct&>(), py::arg("media"))

        .def(py::init<const std::string&, const std::string&, const std::string&>(), py::arg("basepath"),
                                                                                py::arg("name"), py::arg("extension"))

        .def(py::init<const std::string&, const std::string&, const std::string&, v_frame_t, v_frame_t, unsigned int>(),
                py::arg("basepath"), py::arg("name"), py::arg("extension"), py::arg("startframe"), py::arg("endframe"), py::arg("frame_padding"))

        .def(py::init<const std::string&, const std::string&, const std::string&, v_frame_t, v_frame_t, unsigned int, const std::vector<v_frame_t>&>(),
                py::arg("basepath"), py::arg("name"), py::arg("extension"), py::arg("startframe"), py::arg("endframe"), py::arg("frame_padding"), py::arg("missing"))

        .def("__repr__", [](py::handle h) -> std::string
        {
            const MediaClip& m = h.cast<MediaClip&>();
            std::stringstream ss;
            ss << "Media <" << m.TemplatedPath() << " - " << m.Duration() << "f@" << m.Framerate() << "fps at 0x" << std::hex << reinterpret_cast<uintptr_t>(h.ptr()) << ">";
            return ss.str();
        })

        .def("basepath", &MediaClip::Path)
        .def("name", &MediaClip::Name)
        .def("extension", &MediaClip::Extension)
        .def("startframe", &MediaClip::FirstFrame)
        .def("endframe", &MediaClip::LastFrame)
        .def("metadata", &MediaClip::Metadata);

    /* Operator */
    py::class_<ParamValue>(m, "ParamValue")
        .def("get_float", &ParamValue::GetFloat)
        .def("get_bool", &ParamValue::GetBool)
        .def("get_int", &ParamValue::GetInt)
        .def("get_string", &ParamValue::GetString);

    py::enum_<Param::TypeDesc>(m, "ParamTypeDesc")
        .value("Float", Param::TypeDesc::Float)
        .value("Int", Param::TypeDesc::Int)
        .value("Boolean", Param::TypeDesc::Boolean)
        .value("String", Param::TypeDesc::String)
        .export_values();

    py::class_<Param>(m, "Param")
        .def("__repr__", [](py::handle h) -> std::string
        {
            const Param& p = h.cast<Param&>();
            std::stringstream ss;
            ss << "Param <" << p.name << " at 0x" << std::hex << reinterpret_cast<uintptr_t>(h.ptr()) <<">";
            return ss.str();
        })

        .def("get_float", &Param::GetFloat)
        .def("get_int", &Param::GetInt)
        .def("get_bool", &Param::GetBool)
        .def("get_string", &Param::GetString)
        .def("set_float", &Param::SetFloat)
        .def("set_int", &Param::SetInt)
        .def("set_bool", &Param::SetBool)
        .def("set_string", &Param::SetString)
        .def("type", [](const Param* self) -> Param::TypeDesc { return self->type; });

    py::class_<Effect>(m, "Effect")
        .def("__repr__", [](py::handle h) -> std::string
        {
            const Effect& e = h.cast<Effect&>();
            std::stringstream ss;
            ss << "Effect <" << e.Name() << " at 0x" << std::hex << reinterpret_cast<uintptr_t>(h.ptr()) << ">";
            return ss.str();
        })

        .def("name", &Effect::Name)
        .def("set_name", &Effect::SetName, py::arg("name"))
        .def("enabled", &Effect::Enabled)
        .def("set_enabled", &Effect::SetEnabled, py::arg("enable"))
        .def("get_value", &Effect::Value, py::return_value_policy::reference)
        .def("set_value", &Effect::SetValue, py::arg("param"), py::arg("value"));

    py::class_<PlaybackSequence, SharedPlaybackSequence>(m, "PlaybackSequence")
        .def(py::init())
        .def("start_frame", &PlaybackSequence::StartFrame)
        .def("end_frame", &PlaybackSequence::EndFrame)
        .def("set_range", &PlaybackSequence::SetRange, py::arg("start"), py::arg("end"))
        .def("has_media", &PlaybackSequence::HasMedia)
        .def("create_track", static_cast<SharedPlaybackTrack (PlaybackSequence::*)(const Sequence::TrackType&)>(&PlaybackSequence::CreateTrack), py::arg("type"), py::return_value_policy::reference)
        .def("remove_track", static_cast<void (PlaybackSequence::*)(const SharedPlaybackTrack&)>(&PlaybackSequence::RemoveTrack), py::arg("track"))
        .def("remove_track", static_cast<void (PlaybackSequence::*)(int, const Sequence::TrackType&)>(&PlaybackSequence::RemoveTrack), py::arg("index"), py::arg("type"))
        .def("video_track", &PlaybackSequence::VideoTrackAt, py::arg("index"), py::return_value_policy::reference)
        .def("audio_track", &PlaybackSequence::AudioTrackAt, py::arg("index"), py::return_value_policy::reference)
        .def("video_tracks", &PlaybackSequence::VideoTracks, py::return_value_policy::reference)
        .def("audio_tracks", &PlaybackSequence::AudioTracks, py::return_value_policy::reference);

    py::enum_<Sequence::TrackType>(m, "TrackType")
        .value("VIDEO", Sequence::TrackType::VIDEO)
        .value("AUDIO", Sequence::TrackType::AUDIO)
        .export_values();

    py::class_<PlaybackTrack, SharedPlaybackTrack>(m, "PlaybackTrack")
        .def("__repr__", [](py::handle h) -> std::string
        {
            const PlaybackTrack& t = h.cast<PlaybackTrack&>();
            std::stringstream ss;
            ss << "PlaybackTrack <" << t.Name() << " at 0x " << std::hex << reinterpret_cast<uintptr_t>(h.ptr()) << ">";
            return ss.str();
        })
        .def("set_name", static_cast<void (PlaybackTrack::*)(std::string&&)>(&PlaybackTrack::SetName), py::arg("name"))
        .def("name", &PlaybackTrack::Name)
        .def("start_frame", &PlaybackTrack::StartFrame)
        .def("end_frame", &PlaybackTrack::EndFrame)
        .def("set_enabled", &PlaybackTrack::SetEnabled, py::arg("enabled"))
        .def("enabled", &PlaybackTrack::Enabled)
        .def("add_item", &PlaybackTrack::AddMedia, py::arg("media"), py::return_value_policy::reference)
        .def("move_item", &PlaybackTrack::MoveItem, py::arg("track_item"), py::arg("frame"))
        .def("item_at", &PlaybackTrack::ItemAt, py::arg("index"), py::return_value_policy::reference)
        .def("items", &PlaybackTrack::Items, py::return_value_policy::reference);

    py::class_<TrackItem, SharedTrackItem>(m, "TrackItem")
        .def("__repr__", [](py::handle h) -> std::string
        {
            const TrackItem& t = h.cast<TrackItem&>();
            std::stringstream ss;
            ss << "TrackItem <" << t.Name() << " at 0x " << std::hex << reinterpret_cast<uintptr_t>(h.ptr()) << ">";
            return ss.str();
        })
        .def("name", &TrackItem::Name)
        .def("timeline_in", &TrackItem::TimelineIn)
        .def("timeline_out", &TrackItem::TimelineOut)
        .def("source_in", &TrackItem::SourceIn)
        .def("source_out", &TrackItem::SourceOut)
        .def("source_media", &TrackItem::GetMedia, py::return_value_policy::reference);
}

} // namespace bindings

VOID_NAMESPACE_CLOSE
