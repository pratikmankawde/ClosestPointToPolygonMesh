/*
 * Author: Pratik Mankawde
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


TEST_F(PolygonMeshTests, ClosestPointInFrontOfPlane) {
	const Location& loc = pm->closest_point(Vec3(0.5f, 0.5f, 0.5f));
	const Vec3 pos = pm->get_spacial_position(loc);
	ASSERT_EQ(pos, Vec3(0.5f, 0.5f, 0.0f));
}

TEST_F(PolygonMeshTests, ClosestPointAtTheTopRightCorner) {
	const Location& loc = pm->closest_point(Vec3(3.5f, 3.5f, 3.5f));
	const Vec3 pos = pm->get_spacial_position(loc);
	ASSERT_EQ(pos, Vec3(1.0f, 1.0f, 0.0f));
}

TEST_F(PolygonMeshTests, ClosestPointOnTheTopEdge) {
	const Location& loc = pm->closest_point(Vec3(0.0f, 2.0f, 0.0f));
	const Vec3 pos = pm->get_spacial_position(loc);
	ASSERT_EQ(pos, Vec3(0.0f, 1.0f, 0.0f));
}


}
