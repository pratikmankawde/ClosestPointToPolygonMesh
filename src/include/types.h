/*
 * Author: pratik
 */
#pragma once

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

// These data types are not essentially geometric vectors, but a vector of 3 values.
// They can be treated as geometric point or geometric vector or just a collection of 3 values
// depending on the requirement

using Vec3 = Eigen::Vector3f;
using Vec3i = Eigen::Vector3i;
using Vec3ui = Eigen::Matrix<std::size_t, 3, 1>;

using BoundingVolume = Eigen::AlignedBox3f;

BOOST_GEOMETRY_REGISTER_POINT_3D(Vec3, float, cs::cartesian, Vec3::x(), Vec3::y(), Vec3::z())
BOOST_GEOMETRY_REGISTER_BOX(BoundingVolume, Vec3, BoundingVolume::min(), BoundingVolume::max())
