## C++ & OpenGL

**To-do**: 
Compute shader vertex processing code for cloth has been written (clothShader.comp), but needs OpenGL 4.2 or greater to
support compute shaders..

### Cloth Simulation

- Basic cloth simulation with gravity, aero-drag, and ground frictional forces 

- Diffuse and Specular maps applied

- WASD + ZX translate keys, CV keys to rotate wrt y-axis

![cloth simulation](https://github.com/Xavierkst/animation/blob/master/gifs/Cloth_sim_2.gif)

### Character model animation 

Skeleton joint construction, and rigging system is applied to construct the wasp model below. 

- Individual joints can be selected and rotated about some degree of freedom using [Dear Imgui Interface](https://github.com/ocornut/imgui) where joints are located 
  with breadth first search down a tree of joint nodes

- Skeleton joint transformations, skin vertex interpolation, and smooth-skin algos are applied onto the model as part of the rigging process

![Wasp Motionless](https://github.com/Xavierkst/animation/blob/master/gifs/wasp_motionless_edited.gif)

- Time steps can be passed into `AnimationPlayer` class, that also supports different extrapolation methods for keyframes and channels as seen in `Keyframe` and `Channel` classes

- `.skin`, `.skeleton`, and `.anim` files are parsed using a `Tokenizer` class

![Wasp Animation](https://github.com/Xavierkst/animation/blob/master/gifs/wasp_animation_edited.gif)

### Particle simulation

- Tiny spherical particles generated, with gravity, drag forces, frictional forces, and collisions with a ground plane

- Adjustable particle creation rate (num particles per second)

- Particles can increase in size, causing drag force to also increase

- collision elasticity can be toggled as well

![Particle Simulation (sped-up 2x)](https://github.com/Xavierkst/animation/blob/master/gifs/Particle_sim_demonstration_2.gif)
