An extremely condensed version of the OpenGL / SDL application framework I'm currently working on, but written in C. The code is very messy and a few things have been modified before being dumped on GitHub. The updates I push to GitHub may be few and far between.

List of features to be implemented:
* Finish the constraint-based skeletal physics system.
* Collision functions between various types of colliders.
* Finish thread pool / tasking system.
* (?) Add an entity struct that contains groups of renderables.
* Sprites should use index buffer objects like models.
* Create binary file formats and clean, shared loading code for the various skeleton structures, renderables, entities and scenes.
* Models should be able to use multiple textures.
* Re-implement billboards.

List of queued tweaks:
* Merge physIsland and scenes.
* Every vertex of every physics object's collider should ideally not be transformed unless it needs to be.
* Quaternions need to be tweaked, as they give different results at different update rates.
* Replace strtok() with a better function.
* Tidy up and unify file parsing functionality. Each file parsing function pretty much needs a major rework. Maybe put them in separate files as well (e.g. textureWrapperIO.c).
* Tidy up and streamline maths functions.
* A large number of functions need to be cleaned up. Look for comments starting with /**.
* Organize source files into folders. Eventually.
* Phase out SDL?

Download the bin folder if you want to give it a go.