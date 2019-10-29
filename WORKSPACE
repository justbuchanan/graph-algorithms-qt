workspace(name = "com_justbuchanan_graph_algorithms_qt")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "com_justbuchanan_rules_qt",
    branch = "master",
    remote = "https://github.com/justbuchanan/bazel_rules_qt",
)

new_local_repository(
    name = "qt",
    build_file = "@com_justbuchanan_rules_qt//:qt.BUILD",
    # path = "/usr/include/qt", # arch
    path = "/usr/include/x86_64-linux-gnu/qt5",  # debian
)
