# wrui

Wrui is a wrapper for UI libraries (like Qt, wxWidgets, dear imgui) with a common C API. Motivations for these are:

* Using C++ APIs from other languages (such as Rust, Haskell, D, etc) is really annoying because of no standard ABI. Using a C API this can bring popular C++ libraries to these lanugages.
* Some people still prefer to code in C because of its simplicity.
* Wrui will only expose one single symbol which is a table of function pointers. There are several upsides to this:
	* Shared libraries don't need to hard-link with a Wrui shared library. It only needs to be sent one single pointer.
	* Possible for a main application to update with bug fixes without affecting plugins.
	* All code wrapping a certain library can be within one shared library. That makes linking of the main application (for langs that uses static linking) much faster as the library can be loaded in run-time instead of linked with.
	* Clearer separation
* Having a common API but the possibility of using different backends make it possible to integrate a UI into another application. Say that you have a game that use OpenGL and know how to use Wrui. Then it should be possible to use dear imgui (example) backend + Wrui inside that application.

## Status

Development is currently in the very early stages of just figuring things out. How the C API should work and the design goals. I'm currently considering something signal based similar to Qt.
Doing it type-safe would be quite hard in C without pre-processing which I would like to avoid. Considering keeping it not type-safe for C and have other langs adding that support if wanted.

## Plan of attack

* Try to implement an API that works for dear imgui and Qt. These APIs are quite different so should be interesting to get a common base.

## Why not use imgui style of programming?

While I really like the way imgui style programming works it's hard (if not close to impossible) to do with existing libraries (like Qt) while it's quite possible to do it the other way around. So I decided to try to go for a basic signal model and see how well it works. This is still an experiment and may actually not work out but time will tell.

## Non-goals

* Having diffrent UI backends to behave exactly the same will likely be very hard. Wrui will make a best effort in doing so but there will likely be areas where something can't be supported in one or another simply because how that library is implemented. The plan here is to provide documenation that states this and with run-time warnings as well.
