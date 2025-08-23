// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Pybind11 */
#include <pybind11/pybind11.h>

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/MediaBridge.h"
#include "VoidUi/Player/PlayerWidget.h"
#include "VoidUi/Project/Project.h"
#include "VoidUi/VoidGlobals.h"

VOID_NAMESPACE_OPEN

namespace py = pybind11;

namespace bindings {

void BindUi(py::module_& m)
{
    m.doc() = "Void UI Module.";

    m.def("active_player", &GetActivePlayer, py::return_value_policy::reference);
    m.def("active_project", []() { return MBridge::Instance().ActiveProject(); }, py::return_value_policy::reference);

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

    /* Project */
    py::class_<Project>(m, "Project")
        .def("add_media", &Project::AddMedia, py::arg("media_clip"));
}

} // namespace bindings

VOID_NAMESPACE_CLOSE
