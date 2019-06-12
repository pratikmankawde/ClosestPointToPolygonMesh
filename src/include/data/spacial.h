/*
 * Author: pratik
 */
#pragma once

#include <algorithm>
#include <array>
#include <iostream>

#include <data/math.h>
#include <types.h>

namespace data {

// TODO Arrange items to align efficiently in memory
struct Location {
		Vec3ui vertex_indices;		// Triangle vertex indices
		Vec3 barycentric_coords;	// Barycentric coordinates of the point
		size_t polygon_index;		// Polygon index
		bool is_valid;
};

const Location INVALID_LOCATION { Vec3ui(), Vec3(), 0, false };

/**
 * This function tries to project a point on a triangle plane and
 * returns the barycentric coordinates of the projected point.
 * @param a_vertex1 first vertex of triangle
 * @param a_vertex2 second vertex of triangle
 * @param a_vertex3 third vertex of triangle
 * @param a_query_point The point to be projected
 * @return Barycentric coordinates of the projected point
 */
inline Vec3 get_barycentric_coords(const Vec3& a_vertex1,
									const Vec3& a_vertex2,
									const Vec3& a_vertex3,
									const Vec3& a_query_point) {
	const Vec3 side1 = a_vertex2 - a_vertex1;
	const Vec3 side2 = a_vertex3 - a_vertex1;
	Vec3 normal = side1.cross(side2);

	double oneOver4ASquared = 1.0 / normal.dot(normal);

	const Vec3 w = a_query_point - a_vertex1;

	Vec3 barycentric_coords;
	barycentric_coords.z() = (side1.cross(w)).dot(normal) * oneOver4ASquared;
	barycentric_coords.y() = (w.cross(side2)).dot(normal) * oneOver4ASquared;
	barycentric_coords.x() = 1.0 - barycentric_coords.y() - barycentric_coords.z();

	return barycentric_coords;
}

inline Vec3 get_closest_point_on_triangle(const Vec3& a_vertex1,
											const Vec3& a_vertex2,
											const Vec3& a_vertex3,
											const Vec3& a_query_point) {

	if (a_query_point == a_vertex1 || a_query_point == a_vertex2 || a_query_point == a_vertex3) {
		return a_query_point;
	}

	Vec3 barycentric_coord = get_barycentric_coords(a_vertex1, a_vertex2, a_vertex3, a_query_point);

	bool u_is_negative = std::signbit(barycentric_coord.x());
	bool v_is_negative = std::signbit(barycentric_coord.y());
	bool w_is_negative = std::signbit(barycentric_coord.z());

	// If none of the barycentric coords. are negative, we got our point
	if (!u_is_negative && !v_is_negative && !w_is_negative && barycentric_coord.sum() == 1.0f) {
		return barycentric_coord;
	}

	// Else the projection of point is outside the triangle
	// We now try to find the closest point either on edge or on vertex itself.

	if (u_is_negative) {
		Vec3 pt2_query = a_query_point - a_vertex2;
		Vec3 side2 = a_vertex3 - a_vertex2;
		float projected_dist = pt2_query.dot(side2) / side2.squaredNorm();
		barycentric_coord.x() = 0.0f;
		barycentric_coord.z() = clamp(projected_dist, 0.0f, 1.0f);
		barycentric_coord.y() = 1.0f - barycentric_coord.z();
	} else if (v_is_negative) {
		Vec3 pt3_query = a_query_point - a_vertex3;
		Vec3 side3 = a_vertex1 - a_vertex3;
		float projected_dist = pt3_query.dot(side3) / side3.squaredNorm();
		barycentric_coord.x() = clamp(projected_dist, 0.0f, 1.0f);
		barycentric_coord.y() = 0.0f;
		barycentric_coord.z() = 1.0f - barycentric_coord.x();
	} else {
		Vec3 pt1_query = a_query_point - a_vertex1;
		Vec3 side1 = a_vertex2 - a_vertex1;
		float projected_dist = pt1_query.dot(side1) / side1.squaredNorm();
		barycentric_coord.y() = clamp(projected_dist, 0.0f, 1.0f);
		barycentric_coord.x() = 1.0f - barycentric_coord.y();
		barycentric_coord.z() = 0.0f;
	}

	return barycentric_coord;
}

inline std::ostream& operator<<(std::ostream& os, const Location& location) {
    os << "Spacial Location:\nVertex Indices: ["<< location.vertex_indices.transpose()
    		<<"]\nBarycentric: [" << location.barycentric_coords.transpose()
			<< "]\nPolygon Index: [" << location.polygon_index<<"]";
    return os;
}
} /* namespace data */

