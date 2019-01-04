.. _environment:

Environment Scene
=================

Every grid-square of the Population Scene possess an 'environment', which consists of three integer parameters in the range 0-255. These are visualized as colour, the three parameters representing red, green and blue values (further details of how this is achieved are available in the REvoSim paper). The environment for the entire grid can thus conveniently be visualised as a raster (bitmapped) 24-bit colour image in the Environmental Scene. Environments can be static (specified by a single raster image), or dynamic (specified by a sequence of raster images).

The default environmental scene consists of a static image with three colour zones. This image can be changed from the Simulation Settings pannel.

.. figure:: _static/populationSceneDefault.png
    :align: center

    Figure 3.4.1 - Default Environmental Scene showing three blocks of colour in vertical strips.

Dynamic environments can be enabled via the Simulation Settings pannel. There are currently three modes of transformation between image sequences, these are:

1. Once - each image in the sequence is shown in order but only once
2. Loop - each image in the sequence is shown in order, after displaying the last image in the sequence the environment loops back to the first image (etc.)
3. Bounce - each image is shown in order, after displaying the last image the order is reversed, until the first image is once again shown, the order is then reversed (etc.)

.. raw:: html

	<div class="figure align-center">
	<video width="695" height="485" loop autoplay controls muted>
    <source src="_static/environmentSceneLooped.mp4" type="video/mp4">
    Your browser does not support the video tag.
    </video>
	<p class="caption">Video 3.4.1 - Run showing environmental scene with four environmental images on a loop.</p>
	</div>

For dynamic environments, the number of simulation-iterations between image updates is configurable but by default 50 (i.e. the environment image advances to the next in sequence every 50 iterations). REvoSim can optionally interpolate between environmental images to provide smooth transitions.
