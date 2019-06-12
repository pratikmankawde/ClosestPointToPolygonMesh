/*
 * Author: pratik
 */

#include <gtest/gtest.h>

#include <data/polygon_mesh.h>
#include <types.h>

using namespace data;

namespace {

class PolygonMeshTests : public ::testing::Test {
	protected:
		virtual void SetUp() override {
			pm = PolygonMesh::create();
			const std::vector<Vec3> vertices = { Vec3(0.0f, 0.0f, 0.0f),
												 Vec3(1.0f, 0.0f, 0.0f),
												 Vec3(1.0f, 1.0f, 0.0f),
												 Vec3(0.0f, 1.0f, 0.0f) };
			const std::vector<PolygonMesh::Polygon> topology = { PolygonMesh::Polygon(0, 1, 2), PolygonMesh::Polygon(0, 2, 3) };
			pm->build_mesh(vertices, topology);
		}

		virtual void TearDown() override {
			// Code here will be called immediately after each test
			// (right before the destructor).
		}
		std::shared_ptr<PolygonMesh> pm ;
};


TEST_F(PolygonMeshTests, CheckClosestPoint) {
	std::shared_ptr<PolygonMesh> pm  = PolygonMesh::create();
//	const std::vector<Vec3> vertices = { Vec3(0.0f, 0.0f, 0.0f),
//										 Vec3(1.0f, 0.0f, 0.0f),
//										 Vec3(1.0f, 1.0f, 0.0f),
//										 Vec3(0.0f, 1.0f, 0.0f) };
//	const std::vector<Vec3ui> topology = { Vec3ui(0, 1, 2), Vec3ui(0, 2, 3) };
//	pm->build_mesh(vertices, topology);
//	const Location loc = pm->closest_point(Vec3(0.5f, 0.5f, 0.5f));
//	const Vec3 pos = pm->get_spacial_position(loc);
//	ASSERT_EQ(pos, Vec3(0.5f, 0.5f, 0.0f));
}

}
