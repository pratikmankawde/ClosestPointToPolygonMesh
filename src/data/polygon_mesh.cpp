/*
 * Author: Pratik
 */
#include <memory>
#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/detail/envelope/interface.hpp>

#include <data/polygon_mesh.h>
#include <data/spacial.h>

using namespace boost::geometry;
using namespace boost::geometry::index;

namespace data {

PolygonMesh::~PolygonMesh()
{
	// TODO Auto-generated destructor stub
}

bool PolygonMesh::build_mesh(const std::vector<Vec3>& a_vertex_locations, const std::vector<Polygon>& a_topology) {
	m_vertices = a_vertex_locations;
	m_topology = a_topology;
	return true;
}

Location PolygonMesh::closest_point(const Vec3& a_query_point) {

	/*
	 * initialize r-tree data structure if not initialized already
	 * query the polygons closest to the query point
	 * Check each one of them for the closest
	 * Return the closest point as location or return invalid location if not found.
	 */
	std::vector<PolygonIndexPair> nearest_polygons;
	nearest_polygons.reserve(16);

	const std::shared_ptr<SpacialQueryCacheData> cached_data = m_spacial_query_cache->get_cached_data(shared_from_this());
	const std::shared_ptr<SpacialQueryCacheData::SpacialCachePolygonTree> tree = cached_data->get_polygon_tree();
	tree->query(nearest(a_query_point, 16), std::back_inserter(nearest_polygons));

	float min_distance = std::numeric_limits<float>::max();

	Location projected_location = INVALID_LOCATION;
	for (PolygonIndexPair polygon_index : nearest_polygons) {
		const Polygon& polygon = m_topology[polygon_index.second];
		Vec3 vertex1 = m_vertices[polygon[0]];
		Vec3 vertex2 = m_vertices[polygon[1]];
		Vec3 vertex3 = m_vertices[polygon[2]];

		// Our barycentric calculation is based on an assumption that the polygon is a triangle
		// if it's not we will triangulate it by triangle-fan technique and evaluate each triangle.

		Vec3 barycentric_coords = data::get_closest_point_on_triangle(vertex1, vertex2, vertex3, a_query_point);
		Vec3 projected_point = Vec3(vertex1 * barycentric_coords.x()
									+ vertex2 * barycentric_coords.y()
									+ vertex3 * barycentric_coords.z());
		float squared_dist = (projected_point - a_query_point).squaredNorm();
		if (squared_dist < min_distance) {
			min_distance = squared_dist;
			projected_location.barycentric_coords = barycentric_coords;
			projected_location.polygon_index = polygon_index.second;
			projected_location.vertex_indices = polygon;
			projected_location.is_valid = true;
		}
	}

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
	const std::shared_ptr<PolygonMesh> polymesh_data = std::dynamic_pointer_cast<PolygonMesh>(m_data);
	poly_index_pair.resize(polymesh_data->get_polygon_count());
	for (size_t index = 0; index < polymesh_data->get_polygon_count(); ++index) {
		const Polygon& vertex_indices = polymesh_data->get_polygon_at(index);
		BoundingVolume bounding_volume;
		// We create a bounding volume for the polygon
		// In case we have non-triangle based mesh, we will iterate over the vertex points and
		// expand our bounding volume with each of them
		bounding_volume.extend(polymesh_data->get_vertex_at(vertex_indices[0]));
		bounding_volume.extend(polymesh_data->get_vertex_at(vertex_indices[1]));
		bounding_volume.extend(polymesh_data->get_vertex_at(vertex_indices[2]));
		poly_index_pair[index] = std::make_pair(bounding_volume, index);
	}
	// We initialize r-tree with all the data at once. This will initialize tree with packing algo.
	m_polygon_tree = std::make_shared<SpacialCachePolygonTree>(poly_index_pair.begin(), poly_index_pair.end());
}
} /* namespace data */

