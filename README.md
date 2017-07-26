An extremely condensed version of the OpenGL / SDL application framework I'm currently working on, but written in C. The code is very messy and a few things have been modified before being dumped on GitHub. The updates I push to GitHub may be few and far between.

My current "to-do list" (from what I can remember, at least) is as follows:
* Add an entity struct that contains groups of renderables.
* Clean up renderable struct and remove any unnecessary member variables. Currently the only real member variables I am not happy with are sprite and hudElement.
* Entities should be grouped together with a camera somehow, e.g. through a scene.
* Cameras should store a projection matrix, or a pointer to one. Or something. Maybe just move all the window size code from gfxProgram over?
* Skeleton / skeletal animation files and file parsing.
* Fix up render and space transform structures and unify all animation / interpolation code.
* Sprites should use index buffer objects like models.
* Sprites all share the exact same starting mesh now, maybe some optimizations can be made...?
* Create an asset handler.
* Renderable / scene files and file parsing.
* Clean up a large number of functions.

Download the bin folder if you want to give it a go.