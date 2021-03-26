# Animation Project (OpenGL)

Skeleton joint construction, and rigging system is applied to construct the wasp model below. 

### Character model joints DOF toggling

- Individual joints can be selected and rotated about some degree of freedom via [Dear Imgui Interface](https://github.com/ocornut/imgui) using BFS algo to locate joints

- Skeleton joint transformations, skin vertex interpolation, and smooth-skin algos are applied onto the model as part of the rigging process

![Wasp Motionless](https://github.com/Xavierkst/animation/blob/master/gifs/wasp_motionless_edited.gif)

### Animation Player 

- Time steps can be passed into `AnimationPlayer` class, that also supports different extrapolation methods for keyframes and channels as seen in `Keyframe` and `Channel` classes

- `.skin`, `.skeleton`, and `.anim` files are parsed using a `Tokenizer` class

![Wasp Animation](https://github.com/Xavierkst/animation/blob/master/gifs/wasp_animation_edited.gif)

### Cloth Simulation

- Basic `Cloth` simulation applying gravity, aero-drag, spring-damping, and frictional forces on the cloth particles

- WASD and CX keys to translate cloth along XZ plane, and up / down.

- Rotation allowed along the cloth's Y-axis

![cloth simulation](https://github.com/Xavierkst/animation/blob/master/gifs/Cloth_demonstration_edited_2.gif)

### General Particle simulation

- Tiny spherical particles generated, with gravity, drag forces, frictional forces, and collisions with a ground plane

- Adjustable particle creation rate (num particles per second)

- Particles can increase in size, causing drag force to also increase

- collision elasticity can be toggled as well

![Particle Simulation (sped-up 2x)](https://github.com/Xavierkst/animation/blob/master/gifs/Particle_sim_demonstration_2.gif)