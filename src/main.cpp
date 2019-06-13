
#include <iostream>
using namespace std;

#include <data/polygon_mesh.h>
#include <types.h>
#include <memory>

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
	cout<<"Location: "<<loc;
	return 0;
}
