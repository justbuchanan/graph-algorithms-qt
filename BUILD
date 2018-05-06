load("//:qt.bzl", "qt_cc_library", "qt_resource")

cc_binary(
    name = "main",
    srcs = ["main.cpp"],
    copts = [
        "-fpic",
    ],
    deps = [
        ":GraphWidget",
        ":StateSpace",
        ":main_qrc",
        "@qt//:qt_widgets",
    ],
)

qt_resource(
    name = "main_qrc",
    qrc_file = "main.qrc",
    deps = [
        "main.qml",
    ],
)

qt_cc_library(
    name = "GraphWidget",
    src = "GraphWidget.cpp",
    hdr = "GraphWidget.h",
    deps = [
        ":AStarSolver",
        ":DijkstraSolver",
        ":RandomSolver",
        ":StateSpace",
        "@qt//:qt_core",
        "@qt//:qt_gui",
        "@qt//:qt_qml",
        "@qt//:qt_quick",
        "@qt//:qt_widgets",
    ],
)

cc_library(
    name = "StateSpace",
    srcs = ["StateSpace.cpp"],
    hdrs = ["StateSpace.h"],
)

cc_library(
    name = "Solver",
    hdrs = ["Solver.h"],
    deps = [":StateSpace"],
)

cc_library(
    name = "RandomSolver",
    srcs = ["RandomSolver.cpp"],
    hdrs = ["RandomSolver.h"],
    deps = [":Solver"],
)

cc_library(
    name = "DijkstraSolver",
    srcs = ["DijkstraSolver.cpp"],
    hdrs = ["DijkstraSolver.h"],
    deps = [":Solver"],
)

cc_library(
    name = "AStarSolver",
    srcs = ["AStarSolver.cpp"],
    hdrs = ["AStarSolver.h"],
    deps = [":Solver"],
)
