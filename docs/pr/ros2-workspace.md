# Libraries from `stepit.repos`, Not Submodules

Branch `ros2-workspace`.

## Goal

Take `serial` and `framed-serial` from `stepit.repos` instead of git submodules, the way a ROS 2 workspace usually gets the packages it builds on.

The submodules nested the libraries inside the module. StepIt Macro, which builds this module together with StepIt Freezer, then found each library twice, and had to pick one copy and check the other matched.

## Decisions

- **`stepit.repos` is the one list of the libraries.** It pins each to a commit, not a branch. `bin/update.sh` imports them with `vcs` into `modules/`, and industrial_ci reads the same file, so a local build and CI use the same commits. Before, CI followed the tips of `ros2` and `main`.
- **`modules/` keeps its name, and git ignores it.** The paths of the docs and of colcon do not change.
- **`vcs import` runs on every `update`.** A library moves to the commit of `stepit.repos`, and keeps any local change. A `vcs import` with `--skip-existing` would leave an old commit in place after a change to `stepit.repos`.
- **The Dockerfile installs `python3-vcstool`.** `update.sh` needs it.

## Left Out

- **framed-serial does not carry `serial` as a submodule.** colcon does not look inside a folder that has a `package.xml`, so a submodule inside framed-serial would never be built.

## Open Items

- StepIt Macro lists the libraries once, in its own `stepit.repos`, at the same commits as this file, and fails to build when they differ. Moving a library means moving it here, in StepIt Freezer, and in StepIt Macro.
