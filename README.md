An extremely condensed version of the OpenGL / SDL application framework I'm currently working on, but written in C. The code is very messy and a few things have been modified before being dumped on GitHub. The updates I push to GitHub may be few and far between.

List of features to be implemented in order of priority (or in reality, in order from the stuff that's easiest or most fun to the more annoying stuff):
* Particle system.
* Multiple textures for models.
* Finish command system.
* Finish scenes / zones.
* Decals.
* Audio system.
* Collision functions between various types of colliders.
* SIMD.
* Finish thread pool / tasking system.
* Actually implement it.
* Binary file formats and clean, shared loading code for the various resource types.

List of potential modifications and ideas:
* Replace strtok() with a better function.
* Research the Featherstone method for object kinematic chains.
* Research post stabilization to prevent drift.
* Every vertex of every physics object's collider should ideally not be transformed unless it needs to be.
* Quaternions may need to be tweaked, as they seem to give different results at different update rates.
* Tidy up and unify file parsing functionality to use binary files. Each file parsing function pretty much needs a major rework. Maybe put them in separate files as well (e.g. textureWrapperIO.c).
* Tidy up and streamline maths functions. In particular, cut back on unnecessary 'pass-by-references' and possibly add support for SIMD intrinsics.
* A large number of functions need to be cleaned up. Look for comments starting with /**.
* Organize source files into folders. Eventually.
* Phase out SDL?

Download the bin folder if you want to give it a go.