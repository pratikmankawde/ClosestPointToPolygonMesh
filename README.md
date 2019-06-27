 \mainpage

 ### This is a minimal and simple implementation of finding closest point to a Polygonmesh.
 
We are using native format for storing polygonmesh data. A simple vector to store all the vertices and
then another vector to store polygon data. For simplicity we are using triangle based mesh.

Libraries used:
 * _Eigen3_ is used as maths library.
 * r-tree implementation is taken from _Boost_.
 * _Tbb_ can be used for parallizing few of the operations.
 * Additionally we are using _googletests_ frameworks for unit testing.
 * _DOxygen_ for generating documentation.

To build the project, you might need to install [CMake] and [Boost](https://github.com/boostorg/boost) first.
While building, CMake downloads googletest repo and build it. So you will need active internet connection.

Then build steps are:
* cd into _build_ directory located inside root directory of project ClosestPoint.
* Run> cmake .. && make -j4 install
* It should build and put all the binaries in build/bin folder.
* You can simply run the executable> ./ClosestPoint
  and you can run tests with command> ./ClosestPointTests
* Documentation is here> ClosestPoint/build/docs/html/index.html


The project use Boost's r-tree implementation as a acceleration structure for closest point queries.
We first find some closest triangles to the query point using the r-tree, then we check these triangles
to locate the actual closest point.

Location of closest point is being stored as three items: [Polygon index, vertex indices inside this polygon, barycentric coordinates with respect to these vertices],
 which can be used as required to evaluate actual point position, surface normal etc.
