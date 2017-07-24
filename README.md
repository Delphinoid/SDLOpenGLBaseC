An extremely condensed version of the OpenGL / SDL application framework I'm currently working on, but written in C. The code is very messy and a few things have been modified before being dumped on GitHub. The updates I push to GitHub may be few and far between.

My current "to-do list" (from what I can remember, at least) is as follows:
* Renderables should store a pointer to the camera that they wish to use when rendering.
* Cameras should store a projection matrix, or a pointer to one.
* Skeleton / skeletal animation files and file parsing.
* Fix up render and space transform structures and unify all animation / interpolation code.
* Add an entity struct that contains groups of renderables.
* Sprites should use index buffer objects like models.
* Create an asset handler.
* Renderable / scene files and file parsing.
* Clean up a large number of functions.

Download the bin folder if you want to give it a go.