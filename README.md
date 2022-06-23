# First Time Setup

After creating a new project based on this template, run immediately run `tools/initialize-project.sh` **before doing anything else**.
It will delete unnecessary placeholder files, configure `CMakeLists.txt` with your project name, and update this README.

---
# Project Title

Project description.

## Pitchfork Layout

This project follows [The Pitchfork Layout](https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs).

>PFL prescribes several directories that should appear at the root of the project tree. Not all of the directories are required, but they have an assigned purpose, and no other directory in the filesystem may assume the role of one of these directories. That is, these directories must be the ones used if their purpose is required.
>
>Other directories should not appear at the root.
>
>[`build/`](build/README.md): A special directory that should not be considered part of the source of the project. Used for storing ephemeral build results. must not be checked into source control. If using source control, must be ignored using source control ignore-lists.
>
>[`src/`](src/README.md): Main compilable source location. Must be present for projects with compiled components that do not use submodules. In the presence of include/, also contains private headers.
>
>[`include/`](include/README.md): Directory for public headers. May be present. May be omitted for projects that do not distinguish between private/public headers. May be omitted for projects that use submodules.
>
>[`tests/`](tests/README.md): Directory for tests.
>
>[`examples/`](examples/README.md): Directory for samples and examples.
>
>[`external/`](external/README.md): Directory for packages/projects to be used by the project, but not edited as part of the project.
>
>[`extras/`](extras/README.md): Directory containing extra/optional submodules for the project.
>
>[`data/`](data/README.md): Directory containing non-source code aspects of the project. This might include graphics and markup files.
>
>[`tools/`](tools/README.md): Directory containing development utilities, such as build and refactoring scripts
>
>[`docs/`](docs/README.md): Directory for project documentation.
>
>[`libs/`](libs/README.md): Directory for main project submodules.
