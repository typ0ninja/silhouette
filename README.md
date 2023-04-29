**** Silhouette Tool ****

Author: Evan Kight
eid: emk943

repo with alternate implementations:
https://github.com/typ0ninja/silhouette/

*** Instructor Survey ***
I affirm, on my honor that I completed all surveys this semester

please see phong_interp branch for implementation that works better for barrel but worse for sphere

This tool is designed to load a model and tesselate and displace edges based on normal to
lessen the loss of normal map illusion at tangent angles.

<<< ==== How to use ==== >>>:

Based on the very good tutorials at http://www.opengl-tutorial.org/

Tesselation boilerplate from https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation

Full Cmake build instructions here: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-1-opening-a-window/

<<< ==== Controls ==== >>>:

WASD to strafe with camera
mouse controls camera

1 will toggle wireframe model mode
2 will toggle reference geometry(the original model)
3 switches to the deformed sphere model
4 switches to the barrel model
    - more models can be added via adding to game files and updating path accordingly in model_paths mpath_base at line 65 in playground.cpp

o and p will increase/decrease outer heavy tesselation threshold
k and l will increase/decrease inner lighter tesselation threshold
+ and - will increase magnitude of deformation

n and m will increase/decrease outer tesselation subdivision
< and > will increase/decrease inner tesselation subdivision

escape will quit the program

also note frame times in logging allowing basic performance comparisons
