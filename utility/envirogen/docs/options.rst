.. _options:

EnviroGen options
=================

Within the Settings dock of EnviroGen are a number of different modes of creating / modifying environments. All can be accessed by changing the chosen tab at the top of this dock. The tabs are introduced below.

Out
---

[Image]

This tab has EnviroGren output options.

:Save images: When checks images are saved as .pngs into a folder called  *EnviroGen_output* in the output folder. If the folder and files it contains already exists, EnviroGen overwrites any older files with file name conflicts.
:Save location: This defines the location in which the above folder is created - it is, by default, the desktop in all operating systems.
:Change: Launches a file dialogue to change the save location.
:Generate for: Defines the number number of environmental images created.
:Image size: This dictates the size of the image files created.
:Environment type: The type of environment generated is defined using this dropdown menu. When changing to any other tab, the selected environment change is changed here to reflect the current tab, and thus when Run is selected on any other tab, that environment type is generated.
:Resize image: If selected this resizes the image to fill the GUI (but does not alter the size of the image that is created, as defined in image size).

Dyn 1
-----

EnviroGen provides two primary models for the creation of dynamic environment. The first employs a user-defined number of seeds of varying colours and sizes, which drift across the environmental image. A Laplacian smooth is used to define the colour between these seed points.

[image]

The following options are provided to control the environment:

:Number: This is the number of seeds which are used to create the environment.
:Max size: This defines the maximum size these seeds can achieve, in pixels.
:Max size velocity: By default the size of the seeds changes over the course of a run. This option dictates the maximum change in size any given iteration in pixels. Setting it to zero will stop the seeds changing in size (by they will still initialise at different sizes).
:Max velocity: This defines the maximum velocity of the seeds in terms of their movement across the grid in pixels/iteration. Note that these are soft limits - the seeds can exceed these, but will generally move towards the maximum in any direction if it is above this figure.
:Overlap blur factor: When two seeds overlap, this option defines the amount of blurring in the overlapping region. A setting of one ensures that overlapping pixels smooth. Larger than this creates a square zone of blurring around the overlapped sections: at very large values this could create changes to the whole environment when seeds overlap.
:Toroidal: When this option is checked, the environment wraps around to avoid boundary effects. Seeds that disappear from one side will appear from the other - the same is true of top and bottom.
:Convergence: This defines the extent to which the Laplacian smooth creates gradients between seeds - the smaller the number, the smoother the gradients (but the longer the smooth takes to covnerge, especially for the first environmental image).
:Speed factor: This is a convenience option allowing the simulation to be changed only in speed: not any other factor. This option is a multiplier for all velocities.
:Universal blur: By default the Laplacian smooth only creates gradients *between* the seeds. When this option is selected the seeds define pixel colours, but then the smoothing smooths over these, as well as the space between seeds.

Dyn 2
-----

[Image]

 
