#include <iostream>
#include <memory>

#include <data/polygon_mesh.h>
#include <types.h>

using namespace std;
using namespace data;

int main(int argc, char **argv) {
	std::shared_ptr<PolygonMesh> pm = PolygonMesh::create();
	const std::vector<Vec3> vertices = { Vec3(0.0f, 0.0f, 0.0f),
											Vec3(1.0f, 0.0f, 0.0f),
											Vec3(1.0f, 1.0f, 0.0f),
											Vec3(0.0f, 1.0f, 0.0f) };

	const std::vector<Vec3ui> topology = { Vec3ui(0, 1, 2), Vec3ui(0, 2, 3) };

	bool status = pm->build_mesh(vertices, topology);
	if (!status) {
		return 0;
	}
	const Location loc = pm->closest_point(Vec3(0.5f, 0.5f, 0.5f));
	Vec3 pos = pm->get_spacial_position(loc);
	cout << "We setup a square mesh at origin with min. pos (0,0,0) and max. (1,1,0).\n"
			" We queried for closest point to location {0.5f, 0.5f, 0.5f}.\n Result is "
			<< loc
			<< "\nPosition(x,y,z) is :["
			<< pos.transpose() << "]";
	;
	return 0;
}
