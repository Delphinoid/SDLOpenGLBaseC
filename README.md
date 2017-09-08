An extremely condensed version of the OpenGL / SDL application framework I'm currently working on, but written in C. The code is very messy and a few things have been modified before being dumped on GitHub. The updates I push to GitHub may be few and far between.

List of features to be implemented:
* Add an entity struct that contains groups of renderables.
* Cameras should contain a pointer to a scene, and the render code should aim at rendering camera views.
* Sprites should use index buffer objects like models.
* Create file formats and loading code for the various skeleton structures, renderables, entities and scenes.
* Create an asset handler.
* Create a proper framerate handler / state interpolator.
* Models should be able to use multiple textures.

List of queued tweaks:
* Models need to store their own skeletal data.
* Unify file loading functionality.
* Clean up renderable struct and remove any unnecessary member variables. Currently the only real member variables I am not happy with are sprite and hudElement.
* Fix up render and space transform structures and unify all animation / interpolation code (the latter is partly done).
* Not happy with the current billboard system, not sure why.
* Not happy with the skeletal animation system, but I don't think I can improve it while staying within the boundaries I've set. The alternative way of doing it is to animate each bone separately (so they all have their own separate animation timelines).
* A large number of functions need to be cleaned up. Look for comments starting with /**.
* Generate a transformation matrix for sprites in rndrGenerateSprite(). Don't use matrices, they should be unnecessary.
* I still don't like passing allTextures into twLoad().
* Organize source files into folders. Eventually.
* Phase out SDL?

Download the bin folder if you want to give it a go.