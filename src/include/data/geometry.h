/*
 * Author: Pratik Mankawde
 */
#pragma once

#include <Eigen/Core>

#include <data/data.h>
#include <data/spacial.h>
#include <types.h>

namespace data {

/**
 * Class representing Geometry entities. All geometry entities have some kind of control points.
 * In case of \ref PolygonMesh, they will be vertices. Ideally this class can be derived to represent
 * all kinds of geometric types like Curves, Nurbs etc.
 * It is derived from \ref Data.
 */
class Geometry : public Data {
	public:
		/**
		 * Standard constructor
		 */
		Geometry();
		virtual ~Geometry();

		/**
		 * Utility method to return vertex/control point count.
		 * @return no. of vertices/control points in the geometry
		 */
		std::size_t get_vertex_count() const {
			return m_vertices.size();
		}
		/**
		 * Utility method to return a vertex/control point a given index
		 * @param index index of vertex in the storage
		 * @return vertex position
		 */
		const Vec3 get_vertex_at(size_t index) const {
			if (index >= 0 && index < m_vertices.size()) {
				return m_vertices[index];
			} else {
				return Vec3();
			}
		}
		/**
		 * All the derived geometries should implement a way to convert spacial location into
		 * coord. space position data.
		 * @param spacial_location Spacial location of point
		 * @return coord. space position
		 */
		virtual Vec3 get_spacial_position(const Location& spacial_location) const = 0;

	protected:
		/**
		 * Simple vector to store the vertex position data
		 */
		std::vector<Vec3> m_vertices;
};

} /* namespace data */

