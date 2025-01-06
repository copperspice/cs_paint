## CsPaint

### Introduction

The CsPaint library is an abstraction of the Vulkan API. This library is intended for software developers who want to
use Vulkan to render graphics from their application. Using this library will reduce the amount of code required to
render graphics on a GPU. The API of CsPaint provides C++ classes which encapsulates the low level Vulkan API.

A Demo program is provided which contains a few meshes that were modeled in Blender and exported as stl files.  All
rendering in the demo is accomplished by only using the classes available in CsPaint.

Currently CsPaint is supported on Linux and Windows.


### System Requirements

Building CsPaint requires a C++20 compiler and a C++20 standard library. You will also need to download and install
the Vulkan SDK from LunarG.

www.lunarg.com/vulkan-sdk/

CMake build files are provided with the source distribution to build this library. The unit test binary executable is
an optional part of the build process.

This library has been tested with clang sanitizer and an extensive industry code review.


### Documentation

Class level documentation for CsPaint is available on the CopperSpice website:

https://www.copperspice.com/docs/cs_paint/index.html


### Presentations

Our YouTube channel contains over 75 videos about C++, programming fundamentals, Unicode/Strings, multithreading,
graphics, CopperSpice, DoxyPress, and other software development topics.

https://www.youtube.com/copperspice

Links to additional videos can be found on our website.

https://www.copperspice.com/presentations.html


### Authors / Contributors

* **Ansel Sermersheim**
* **Barbara Geller**


### License

This library is released under the BSD 2-clause license. For more information refer to the LICENSE file provided with
this project.


### References

 * Website:  https://www.copperspice.com
 * Twitter:  https://twitter.com/copperspice_cpp
 * Email:    info@copperspice.com

<!-- -->
 * Github:   https://github.com/copperspice

<!-- -->
 * Forum:    https://forum.copperspice.com
 * Journal:  https://journal.copperspice.com

