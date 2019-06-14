/*
 * Author: Pratik Mankawde
 */
#pragma once

#include <algorithm>
#include <array>
#include <iostream>

#include <data/math.h>
#include <types.h>

namespace data {

/**
 * Spacial Location of projected point on mesh (The point on mesh can be thought of a projection of query point).
 * This can be represented using 3 attributes, which can be used as per the requirement.
 * - vertex_indices 	: They represent indices of vertices of triangle,
 * 						inside which(or on one of the vertex itself or on one of its edge) projected point is located.
 * - polygon_index		: Index of polygon(triangle in our case as our mesh is triangle based),
 * 						on which projected point is located
 * - barycentric_coords	: barycentric coordinates of the projected point
 * 						with ref. to the three vertices in \ref vertex_indices
 * - in_valid			: flag representing if the Location is valid.
 */
struct Location {
		Vec3ui vertex_indices;		// Indices of vertices inside polygon(triangle in our case)
		size_t polygon_index;		// Polygon index
		Vec3 barycentric_coords;	// Barycentric coordinates of the point
		bool is_valid;				// Flag representing if the location is valid
};

/**
 * Const to represent an invalid location
 */
static const Location INVALID_LOCATION { Vec3ui(), 0, Vec3(), false };

/**
 * This function tries to project a point on a triangle plane and
 * returns the barycentric coordinates of the projected point.
 * Implementation is taken from <a href="https://pdfs.semanticscholar.org/0141/b1416bb749bb5ba94210a30d70f0824760a4.pdf">here</a>.
 * Title: Computing the Barycentric Coordinates of a Projected Point (Preprint of an Article in the Journal of Graphics Tools)
 *  by Wolfgang Heidrich, The University of British Columbia
 *
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
	// We have simplified the calculations here
	float one_over_4area_sq = 1.0 / normal.dot(normal);

	const Vec3 vertex_to_querypt = a_query_point - a_vertex1;

	Vec3 barycentric_coords;
	barycentric_coords.z() = (side1.cross(vertex_to_querypt)).dot(normal) * one_over_4area_sq;
	barycentric_coords.y() = (vertex_to_querypt.cross(side2)).dot(normal) * one_over_4area_sq;
	barycentric_coords.x() = 1.0 - barycentric_coords.y() - barycentric_coords.z();

	return barycentric_coords;
}

/**
 * ###This function finds closest point on a triangle given a query point.
 *
 * We first find barycentric coords.(BCs) of projection of query point on triangle plane.
 * If all the BCs are positive, it means projected point(PP) is inside the triangle,
 * hence this is the closest point on triangle to the query point.
 * Else projected point is outside the triangle and hence closest point can be on an edge or on one of the vertex.
 * Let's say our triangle has vertices [a, b, c], if let's say BC.x is negative,
 * it would mean projected point it beyond the edge opposite to first vertex(a) of triangle.
 * Hence closest projected point can be either on edge b-c or on b or c.
 * We re-project(RP) the projected point(PP) on to the edge b-c and then find ratio of it's distance from b and c.
 * This would give us good estimate of where the closest point lies. If the latest projected point(RP) is not between
 * b and c but on an extended line b-c, we clamp it to b or c depending on which side it is.
 *
 * @param a_vertex1 1st vertex of triangle
 * @param a_vertex2 2st vertex of triangle
 * @param a_vertex3 3st vertex of triangle
 * @param a_query_point point for which closest point is to be calculated
 * @return Closest point to query_point on the triangle
 */
inline Vec3 get_closest_point_on_triangle(const Vec3& a_vertex1,
											const Vec3& a_vertex2,
											const Vec3& a_vertex3,
											const Vec3& a_query_point) {

	// A minor optimisation to return if the query point is coinciding with any of the triangle vertex
	if (a_query_point == a_vertex1 || a_query_point == a_vertex2 || a_query_point == a_vertex3) {
		return a_query_point;
	}

	// Get the BC of the projected point
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
		// Point either lies on edge b-c or on b or c
		Vec3 vertex2_query = a_query_point - a_vertex2;
		Vec3 side2 = a_vertex3 - a_vertex2;
		float projected_dist = vertex2_query.dot(side2) / side2.squaredNorm();
		barycentric_coord.x() = 0.0f;
		barycentric_coord.z() = clamp(projected_dist, 0.0f, 1.0f);
		barycentric_coord.y() = 1.0f - barycentric_coord.z();
	} else if (v_is_negative) {
		// Point either lies on edge c-a or on a or c
		Vec3 vertex3_query = a_query_point - a_vertex3;
		Vec3 side3 = a_vertex1 - a_vertex3;
		float projected_dist = vertex3_query.dot(side3) / side3.squaredNorm();
		barycentric_coord.x() = clamp(projected_dist, 0.0f, 1.0f);
		barycentric_coord.y() = 0.0f;
		barycentric_coord.z() = 1.0f - barycentric_coord.x();
	} else {
		// Point either lies on edge a-b or on a or b
		Vec3 vertex1_query = a_query_point - a_vertex1;
		Vec3 side1 = a_vertex2 - a_vertex1;
		float projected_dist = vertex1_query.dot(side1) / side1.squaredNorm();
		barycentric_coord.y() = clamp(projected_dist, 0.0f, 1.0f);
		barycentric_coord.x() = 1.0f - barycentric_coord.y();
		barycentric_coord.z() = 0.0f;
	}

	return barycentric_coord;
}

/**
 * Utility function to convert Location into printable string
 * @param os output stream
 * @param location spacial location object
 * @return
 */
inline std::ostream& operator<<(std::ostream& os, const Location& location) {
	os << "Spacial Location:\nVertex Indices: [" << location.vertex_indices.transpose()
		<< "]\nBarycentric: ["
		<< location.barycentric_coords.transpose()
		<< "]\nPolygon Index: ["
		<< location.polygon_index << "]";
	return os;
}
} /* namespace data */

