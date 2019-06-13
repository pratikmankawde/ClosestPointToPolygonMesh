 \mainpage

 ### This is a minimal and simple implementation of finding closest point to a Polygonmesh.
 
We are using native format for storing polygonmesh data. A simple vector to store all the vertices and
then another vector to store polygon data. For simplicity we are using triangle based mesh.

Libraries used:
 * We are using _googletests_ frameworks for unit testing.
 * _DOxygen_ for generating documentation.
 * Additionally _Eigen3_ is used as maths library.
 * r-tree implementation is taken from _Boost_.
 * _Tbb_ can be used for parallizing few of the operations.