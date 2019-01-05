---
# Colorize_grow Project, C++

## **Deprecated/ancient school project.**

This is one route I took for an image colorization 
project, wherein the user takes a greyscale image,
creates a colored, boundary-guided "color profile" for it,
and the program then grows those colors to fill said 
boundaries.

The program then takes the a user-provided alpha-mask
(by default, using the inverse original user image)
and uses this to specify the intensity of the colors
at specific areas of the image.

This was a rushed version of this type of algorithm;
optimally, it would work properly if combined with 
a smart-masking system.

----
## Requirements

NetPBM Suite (installed on most Linux distributions by default), make and G++/GCC.
----
