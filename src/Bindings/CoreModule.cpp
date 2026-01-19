// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Pybind11 */
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

/* Internal */
#include "Definition.h"
#include "VoidCore/Media/Filesystem.h"
#include "VoidObjects/Media/MediaClip.h"

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

    /* Frame */
    py::class_<Frame>(m, "Frame")
        .def(py::init<const std::string&>(), py::arg("path"))
        .def("fullpath", &Frame::Fullpath)
        .def("basepath", &Frame::Basepath)
        .def("name", &Frame::Name)
        .def("extension", &Frame::Extension)
        .def("framenumber", &Frame::Framenumber)
        .def("is_single_file", &Frame::SingleFile)
        .def("is_valid", &Frame::Valid);

    /* MediaStruct */
    py::class_<MediaStruct>(m, "MediaStruct")
        .def(py::init<Frame&, const MediaType&>(), py::arg("media_entry"), py::arg("media_type"))
    
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

    // /* Frame */
    // py::class_<Frame>(m, "Frame")
    //     .def(py::init<const MEntry&>(), py::arg("media_entry"))
    //     .def("path", &Frame::Path)
    //     .def("framenumber", &Frame::Framenumber);

    // py::class_<MovieFrame>(m, "MovieFrame")
    //     .def(py::init<const MEntry&, const v_frame_t>(), py::arg("media_entry"), py::arg("frame"))
    //     .def("path", &Frame::Path)
    //     .def("framenumber", &Frame::Framenumber);

    /* Media */
    py::class_<Media>(m, "Media")
        .def(py::init<const MediaStruct&>(), py::arg("media_struct"))

        .def(py::init<const std::string&, const std::string&, const std::string&, v_frame_t, v_frame_t, unsigned int>(),
                py::arg("basepath"), py::arg("name"), py::arg("extension"), py::arg("startframe"), py::arg("endframe"), py::arg("frame_padding"));

        // .def("get_frame", &Media::GetFrame, py::arg("frame"));

    /* MediaClip */
    py::class_<MediaClip, SharedMediaClip>(m, "MediaClip")
        .def(py::init<const MediaStruct&>(), py::arg("media"))

        .def(py::init<const std::string&, const std::string&, const std::string&>(), py::arg("basepath"),
                                                                                py::arg("name"), py::arg("extension"))

        .def(py::init<const std::string&, const std::string&, const std::string&, v_frame_t, v_frame_t, unsigned int>(),
                py::arg("basepath"), py::arg("name"), py::arg("extension"), py::arg("startframe"), py::arg("endframe"), py::arg("frame_padding"))

        .def(py::init<const std::string&, const std::string&, const std::string&, v_frame_t, v_frame_t, unsigned int, const std::vector<v_frame_t>&>(),
                py::arg("basepath"), py::arg("name"), py::arg("extension"), py::arg("startframe"), py::arg("endframe"), py::arg("frame_padding"), py::arg("missing"))

        .def("basepath", &MediaClip::Path)
        .def("name", &MediaClip::Name)
        .def("extension", &MediaClip::Extension)
        .def("startframe", &MediaClip::FirstFrame)
        .def("endframe", &MediaClip::LastFrame)
        .def("metadata", &MediaClip::Metadata);
}

} // namespace bindings

VOID_NAMESPACE_CLOSE
