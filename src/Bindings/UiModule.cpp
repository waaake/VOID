// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Pybind11 */
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

/* Internal */
#include "Definition.h"
#include "VoidCore/Serialization.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Player/PlayerWidget.h"
#include "VoidUi/Media/MetadataViewer.h"
#include "VoidUi/Project/Project.h"
#include "VoidUi/Engine/Globals.h"

VOID_NAMESPACE_OPEN

namespace py = pybind11;

namespace bindings {

void BindUi(py::module_& m)
{
    m.doc() = "Void UI Module.";

    m.def("active_player", &UIGlobals::GetActivePlayer, py::return_value_policy::reference);
    m.def("active_project", []() { return MBridge::Instance().ActiveProject(); }, py::return_value_policy::reference);
    m.def("load_project", [](const std::string& path) { MBridge::Instance().Load(path); }, py::arg("path"));
    m.def("metadata_viewer", &UIGlobals::GetMetadataViewer, py::return_value_policy::reference);

    /* Player */
    py::class_<Player>(m, "Player")
        .def("play_forwards", &Player::PlayForwards)
        .def("play_backwards", &Player::PlayBackwards)
        .def("play", &Player::PlayForwards)
        .def("stop", &Player::Stop)
        .def("next_frame", &Player::NextFrame)
        .def("previous_frame", &Player::PreviousFrame)
        .def("move_to_start", &Player::MoveToStart)
        .def("move_to_end", &Player::MoveToEnd)
        .def("set_frame", &Player::SetFrame, py::arg("frame"))
        .def("load", py::overload_cast<const SharedMediaClip&>(&Player::Load), py::arg("media_clip"));

    /* Metadata Viewer */
    py::class_<MetadataViewer>(m, "MetadataViewer")
        .def("set_from_media", &MetadataViewer::SetFromMedia)
        .def("set_metadata", &MetadataViewer::SetMetadata);

    /* Project */
    py::class_<Project> project(m, "Project");

    /* Project Save Type*/
    py::enum_<EtherFormat::Type>(project, "EtherFormat")
        .value("Ascii", EtherFormat::Type::ASCII)
        .value("Binary", EtherFormat::Type::BINARY)
        .export_values();

    project
        .def("add_media", &Project::AddMedia, py::arg("media_clip"))
        .def("document", &Project::Document, py::arg("name"))
        .def("modified", &Project::Modified)
        .def(
            "save",
            static_cast<bool (Project::*)(const std::string&, const std::string&, const EtherFormat::Type&)>(&Project::Save),
            py::arg("path"), py::arg("name"), py::arg("type")
        );
}

} // namespace bindings

VOID_NAMESPACE_CLOSE
