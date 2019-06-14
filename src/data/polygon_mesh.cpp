/*
 * Author: Pratik Mankawde
 */
#include <memory>
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/detail/envelope/interface.hpp>
#include <tbb/parallel_for.h>

#include <data/polygon_mesh.h>
#include <data/spacial.h>
#include <types.h>

using namespace boost::geometry;
using namespace boost::geometry::index;

namespace data {

bool PolygonMesh::build_mesh(const std::vector<Vec3>& a_vertex_locations, const std::vector<Polygon>& a_topology) {

	// Ideally we should check each polygon for various factors, like Delaunay triangulation etc,
	// depending on the use case, once verified can we can save them
	if (a_vertex_locations.size() < 3 || a_topology.size() < 1) {
		return false;
	}

	m_vertices = a_vertex_locations;
	m_topology = a_topology;
	return true;
}

Location PolygonMesh::closest_point(const Vec3& a_query_point) const {

	/** Steps followed to calculate closest point:
	 *
	 * - Initialize r-tree data structure if not initialized already
	 * - query the polygons closest to the query point
	 * - Check each one of them for the closest projected point on them.
	 * - Return the closest point as spacial location or return invalid location if not found.
	 */

	if (m_vertices.size() < 3) {
		return INVALID_LOCATION;
	}

	std::vector<PolygonIndexPair> nearest_polygons;
	nearest_polygons.reserve(CLOSEST_TRIANGLE_COUNT);

	const std::unique_ptr<SpacialQueryCacheData>& cached_data =
			m_spacial_query_cache->get_cached_data(shared_from_this());
	const std::unique_ptr<SpacialQueryCacheData::SpacialCachePolygonTree>& tree = cached_data->get_polygon_tree();

	// We query for 16 polygons closest to our query point and then we check each of them individually
	tree->query(nearest(a_query_point, CLOSEST_TRIANGLE_COUNT), std::back_inserter(nearest_polygons));

	Location projected_location = INVALID_LOCATION;
	std::vector<float> squared_distances;
	squared_distances.resize(CLOSEST_TRIANGLE_COUNT);

	std::vector<Vec3> barycentric_coords_vec;
	barycentric_coords_vec.resize(CLOSEST_TRIANGLE_COUNT);

	tbb::parallel_for(std::size_t { 0 }, CLOSEST_TRIANGLE_COUNT, [&](std::size_t index) {
		PolygonIndexPair& polygon_index_pair = nearest_polygons[index];
		const Polygon& polygon = m_topology[polygon_index_pair.second];
		const Vec3& vertex1 = m_vertices[polygon[0]];
		const Vec3& vertex2 = m_vertices[polygon[1]];
		const Vec3& vertex3 = m_vertices[polygon[2]];

		// Our barycentric calculation is based on an assumption that the polygon is a triangle
		// if it's not we will triangulate it here by triangle-fan technique and evaluate each triangle.
		const Vec3 barycentric_coords = data::get_closest_point_on_triangle(vertex1, vertex2, vertex3, a_query_point);
		const Vec3 projected_point = Vec3(vertex1 * barycentric_coords.x()
				+ vertex2 * barycentric_coords.y()
				+ vertex3 * barycentric_coords.z());
		squared_distances[index] = (projected_point - a_query_point).squaredNorm();
		barycentric_coords_vec[index] = barycentric_coords;
	});

	size_t closest_location_index = 0;
	float min_distance = std::numeric_limits<float>::max();
	for (size_t index = 0; index < CLOSEST_TRIANGLE_COUNT; ++index) {
		if (squared_distances[index] < min_distance) {
			min_distance = squared_distances[index];
			closest_location_index = index;
		}
	}

	projected_location.barycentric_coords = barycentric_coords_vec[closest_location_index];
	PolygonIndexPair& polygon_index_pair = nearest_polygons[closest_location_index];
	projected_location.polygon_index = polygon_index_pair.second;
	projected_location.vertex_indices = m_topology[polygon_index_pair.second];
	projected_location.is_valid = true;

	return projected_location;
}

Vec3 PolygonMesh::get_spacial_position(const Location& spacial_location) const {
	if (!spacial_location.is_valid) {
		return Vec3();
	}
	return spacial_location.barycentric_coords.x() * m_vertices[spacial_location.vertex_indices[0]]
			+ spacial_location.barycentric_coords.y() * m_vertices[spacial_location.vertex_indices[1]]
			+ spacial_location.barycentric_coords.z() * m_vertices[spacial_location.vertex_indices[2]];
}

void PolygonMesh::SpacialQueryCacheData::compute() {

	if (static_cast<bool>(m_polygon_tree)) {
		m_polygon_tree->clear();
	}
	std::vector<PolygonIndexPair> poly_index_pair;
	const std::shared_ptr<const PolygonMesh> polymesh_data = std::dynamic_pointer_cast<const PolygonMesh>(m_data);
	poly_index_pair.resize(polymesh_data->get_polygon_count());

	tbb::parallel_for(std::size_t { 0 }, polymesh_data->get_polygon_count(), [&](std::size_t index) {
		const Polygon& vertex_indices = polymesh_data->get_polygon_at(index);
		BoundingVolume bounding_volume;
		// We create a bounding volume for the polygon
		// In case we have non-triangle based mesh, we will iterate over the vertex points and
		// expand our bounding volume with each of them
		bounding_volume.extend(polymesh_data->get_vertex_at(vertex_indices[0]));
		bounding_volume.extend(polymesh_data->get_vertex_at(vertex_indices[1]));
		bounding_volume.extend(polymesh_data->get_vertex_at(vertex_indices[2]));
		poly_index_pair[index] = std::make_pair(bounding_volume, index);
	});
	// We initialize r-tree with all the data at once. This will initialize tree with packing algo.
	m_polygon_tree = std::unique_ptr<SpacialCachePolygonTree>(new SpacialCachePolygonTree(	poly_index_pair.begin(),
																							poly_index_pair.end()));
}
} /* namespace data */

