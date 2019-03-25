# VAPID - Stupid Fast Build System

Content builds for video games stress every aspect of a build system. They involve hundreds of gigabytes of data, hundreds of thousands of files, and have dozens of compiler steps that chain together to produce outputs, each with multiple inputs and outputs. The end users are non-programmers (artists and designers) and require easy to read progress and error messages.

The very basic problem of making sure your build system has a fast start up time for an incremental build (e.g. a user changes a single file) is complicated when you have to parse 300k input/output files totaling 500 gb to see if they have changed to find that single file to compile, which is how most build systems work. The fastest SSD can still take minutes to perform this step. This is a non-issue on the even the largest code base and hence why current build systems are not capable of this task

But, if these issues of building at a massive scale can be solved for a content build system, then it will be shrug off even the most demanding code build needs.

Build systems are also not very sexy. Despite the incredible power and productivity impacts, the inherent hard problems they need to solve end up with hard interfaces to learn in order to use them, making all but the most technical people avoid until absolutely necessary.

This is where the name 'vapid' comes from. It should be dumb enough that you should be able to learn the gist of it by a glance at the configuration file. And it sounds like 'rapid' which is a good build system quality to have.

# Core Design Goals

* Windows as a primary platform. Almost all AAA game development happens on windows, yet most build system development seems to  happens on Linux.
* Distributed Building. It can take 4-24 hours to build content for a AAA game on a single machine, being able to throw machines at this to reduce compile times should be an option.
* Cached Compiling. Most content does not change every compile, users updating to the latest content at the start of the day, with no local changes, should be able to simply download the current build. This can also take a big bite into that 4-24 hour content build time. A very robust caching system could also serve as a solution for distributing daily builds to a team, and ensuring they are always in sync with code and content (a recurring issue at every game company).
* No Extra work to find implicit dependencies. Having to write separate processes to scan for implicit dependencies is extermely error prone, and slows the build down overall 
* Prioritize iteration time over full-build time. Most users operate on one file of the build system, not all of them. Full build times are expected to be improved just by the distribution and cached aspects.
* Robust GUI based progress/error solution. This is specific to content users, but when builds take a while users need to know what is taking so long, and if errors occur, they need to be highlighed in a direct an obvious manner so they know what action needs to be taken. There should be 'share in slack' buttons for errors when people can't figure out why their build is broken which posts all of the required information.
* Learn it in 15 minutes. No custom configuration language, and prefer simple syntax over features.

# What about other existing open source build systems?

Before this undertaking I spent a lot of time evaluating existing open source build systems, and seeing how they would work with games my company is working on. But all of them are focused on building code, not content, and are missing critical features as a result.

The biggest problem with existing build systems is you need full support for implicit dependencies. Code build systems generally have a /showIncludes support to query implicit dependencies from a C++ file as some sort of reluctant hack added later on during their development. But this breaks down when those dependencies are generated, which is often the case with content in several stages.

At work we currently use WAF, which is missing almost every core feature I desire from vapid, but is still the only open source build system that I know of that has implicit dependency support that is robust enough to handle content builds.

# Usage Overview

Since the goal is to learn how to use vapid in a glance of the configuration script, this is a configuration script:

```
import vapid

config = vapid.option('config', default='debug', help='What configuration to use when compiling')

def make_cxx_exe(name, source)
  objs = vapid.compile(run="cl /c $ /Fo:build/%s/$.obj" % (config), source=source)
  vapid.link(run="link /Fo:build/%s/%s.exe $" % (config, name), source='build/%s/%s.obj' % (config, source), dependencies=[objs])
  
make_cxx_exe('game.exe', 'source/game/**/*.cpp')
make_cxx_exe('editor.exe', 'source/editor/**/*.cpp')
```

This should compile all of the cpp files from the source folder and link into two executables. Vapid contains three functions:
* `option` - Specify a compiler option for vapid to parse.
* `compile` - the rule is called for every file in source.
* `link` - the rule is called once, with $ being all of the input files in separate arguments.

These functions return an identifier for this task which can be passed as a dependency of a later rule, which allows vapid to run these tasks in parallel.

Because the configuration script is just python, all other support, like release/debug builds, unity builds, include paths, and platform support will need to be implemented in the python script itself. This is intended to be done by wrapping these vapid calls in functions that would generate the command line itself, like in the above example.

For content builds, it could look like:

```
import vapid

package = vapid.option('package', default=False, help='Should package assets into distribtion archives')
platform = vapid.option('platform', default='pc', values=['pc', 'ps4', 'xb1'], help='What platform to build for')

tga_textures = vapid.compile(run="texc -i $ -o build/$.dds", source="content/textures/**/*.tga")
png_textures = vapid.compile(run="texc -i $ -o build/$.dds", source="content/textures/**/*.png")
# note only audio has to be encoded differently per platform
audio = vapid.compile(run="wavc -p %s -i $ -o build_%s/$c" % (platform, out_dir), source="content/audio/**/*.wav")
mesh = vapid.compile(run="meshc -i $ -o build/$c", source="content/meshes/**/*.mesh")
# levels may reference meshes, and depend on mesh output files
levels = vapid.compile(run="levelc -i $ -o build/$c", source="content/levels/**/*.level", dependencies=[mesh])

if package: 
  vapid.link(run="7z a assets.7z $", source=["build/content/**/*", "build_%s/content/**/*" % (platform)], dependencies=[tga_textures, png_textures, audio, mesh, levels])
```

# Incremental Builds

To recompile a texture you should be able to specify:

`vapid content/meshes/wall.mesh`

Vapid will match this file path against all of the rules that were specified, which should be nearly instantanous from a user perspective, and call any rules affected. Any previous tasks that read the outputs from this task (.level files) will also be re-run. Any tasks that might have to re-run because they reference the output paths explicitly will also run (e.g. if `--package` was specified, and a new .mesh was added, it would get packaged) 

Vapid does not generate a backwards-graph, like 'make' or other build systems. Instead it just runs tasks in order as they are specified in the configuration file, with the `dependencies` parameter specifying synchronization points for parallelism.

A backwards graph is faster for full builds, but for content builds this is not what we care about 99% of the time. Vapid achieves fast full build times through distributed compiling and caching.

This forward-declaration syntax is also what allows the build system syntax to be so simple. Part of the reason I think so many people do not like to touch build systems configuration files is the inherent friction between wanting to declare things in a forward syntax (compile my .cpp file into an obj and link it with the game!) vs how you actually get that to happen (my game.exe link step depends on an .obj file, that obj is produced by a link step which depends on my .cpp file).

# Implementation Details

There are four processes that need to run vapid at an enterprise level:

* A cache server, which serves precompiled task outputs. This will be configured to store at least one cached output for each task, and store history up to a specific cache size.
* A worker manager, which coordinates availble workers
* A dedicated worker, run one of these per core on every dedicated build machine you have, and connects to the worker manager.
* A build monitor, this is a GUI program that runs on the user machine and remains active on the task bar, it connects to the cache server and worker manager. When you run a build, it then starts a bunch of worker processes locally, parses the build configuration, and starts requesting workers to perform tasks. This may also be configured to use a couple cores of the user machines to perform build tasks for anybody at the company if desired, possibly only activating this when the machine has been idle for a while. This may be an increasingly viable option as many core systems are becoming available (e.g. threadripper).

The build monitor may be triggered by a command line application for integration into code build tools, but you can see a GUI representation of the progress looking at the output.

When run on a personal project level, you only need to run the build monitor, which will then just ignore the cache server if not present (using a local cache), and use only the local build workers.

* The key to it working is it uses Microsoft Detours, a dependency-injection library to capture disk IO calls from a process.
* Vapid then re-routes all IO traffic to read/write files over a network, allowing the compilation to be distributed while reading/writing from the host machine.
* This also captures all input and output filenames, so implicit dependencies and output filenames are tracked for that task.
* The cache server serves the same purpose remotely as locally, comparing your file hashes against the previously run file hashes, and if they match, it uses this file, if not, it requests a build worker to build it.
* Vapid will use MD5 or another hash algorithm to determine if a file has changed for cache purposes, and also hashes the command line, so you can safely incrementally alter the configuration file.

