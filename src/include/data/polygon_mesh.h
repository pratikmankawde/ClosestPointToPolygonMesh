/*
 * Author: Pratik Mankawde
 */
#pragma once

#include <cmath>
#include <vector>
#include <math.h>
#include <memory>

#include <boost/geometry/index/rtree.hpp>

#include <data/cache.h>
#include <data/geometry.h>
#include <data/spacial.h>
#include <types.h>

using namespace boost::geometry;
using namespace boost::geometry::index;

namespace data {

/**
 * This class represents a Polygon mesh.
 *
 * Vertices are stored directly into a vector in Geometry class. Topology is stored separately here.
 * Each entry in topology is a collection of vertices of a polygon. We are assuming a triangle based mesh here.
 * We can have generalised version of polygon mesh as this one's parent class and then specialised triangle based
 * mesh implemented here. We are keeping this simple for now. This class contain logic for various evaluation functions.
 */
class PolygonMesh : public Geometry, public std::enable_shared_from_this<PolygonMesh> {
	public:

		virtual ~PolygonMesh() = default;

		/**
		 * We are aliasing Polygon for readability. Polygon here represents set of vertices constructing the polygon.
		 */
		using Polygon = Vec3ui;

		/**
		 * We aliasing PolygonIndexPair to represent pair of BoundingBox of vertices and polygon index in mesh
		 */
		using PolygonIndexPair = std::pair<BoundingVolume, size_t>;

		/**
		 * @param a_vertex_locations 	Vertex locations
		 * @param a_topology			Topology of polygon mesh, set of vertices representing polygon.
		 * 								Ex.: {{1,2,3}, {2,3,4,5}, {3,6,7,4}}, for our case it will always be
		 * 								a set of 3 vertices. We can extent this so that a polygon can carry
		 * 								any no. of vertices. For storing more than 3 vertex indices,
		 * 								it would be advisable to implement some kind of block_array type,
		 * 								which can sit on stack instead of heap for efficient and faster access.
		 * @returns true if mesh was built successfully else false.
		 */
		bool build_mesh(const std::vector<Vec3>& a_vertex_locations, const std::vector<Polygon>& a_topology);

		/**
		 * Given a point in same space as mesh, this function finds its closest point on the mesh.
		 * We are using boost::geometry::r-tree as the acceleration structure. It stores bounding boxes
		 * of set of polygons and associated index of polygon in it's leaf. It is a balanced tree hence giving faster
		 * lookup time.
		 * @param a_query_point Point to query for
		 * @return Spacial location of closest point.
		 */
		Location closest_point(const Vec3& a_query_point) const;

		/**
		 * Gets vertex position(x,y,z) given spacial location.
		 * It uses barycentric coordinates and vertex positions of the triangle
		 * this spacial_location object represents to calculate object space vertex coord.
		 * @param spacial_location Spacial location of the point
		 * @return Vertex positions in (x,y,z) coord. form
		 */
		Vec3 get_spacial_position(const Location& spacial_location) const override;

		/**
		 * Gets the no. of polygons this polygon mesh has.
		 * @return Polygon count
		 */
		size_t get_polygon_count() const {
			return m_topology.size();
		}

		/**
		 * Fetches the indices of vertices constituting this polygon
		 * @param index polygon index
		 * @return Vertex indices of asked polygon
		 */
		inline const Polygon get_polygon_at(size_t index) const {
			if (index >= 0 && index < m_topology.size()) {
				return m_topology[index];
			} else {
				return Polygon();
			}
		}

		// factory function that creates shared ptr for Polygonmesh
		/**
		 * Creates a shared pointer for this, required for enable_shared_from_this
		 * to work properly.
		 * @return shared_ptr of PolygonMesh type
		 */
		static std::shared_ptr<PolygonMesh> create() {
			return std::shared_ptr<PolygonMesh>(new PolygonMesh());
		}

	private:

		/**
		 * Constructor is made private so that one always create PolygonMesh object through `create()` method
		 * making sure we have a shared_pre alive and enable_shared_from_this would work
		 */
		PolygonMesh()
					: Geometry()
					, m_spacial_query_cache(std::make_shared<Cache<SpacialQueryCacheData>>()) {};

		/**
		 * We are assuming mesh is triangle based, hence we will have only 3 vertices defining a polygon.
		 * In cases where polygon has more than 3 vertices we can handle it in two ways:
		 * 1. We can triangulate the mesh
		 * 2. We can define topology as std::vector<block>, where 'block' datatype can simply be
		 *	  described as std::vector<std::size_t>. Alternatively we can implement it as a block array data structure,
		 *	  may be stack allocated, if that can give us some performance improvement.
		 * For simplicity we will only handle a triangle based mesh for now. We are using general data type
		 * we defined in \ref types.h \ref Vec3ui to store index of vertices in the polygon.
		 */
		std::vector<Polygon> m_topology;

		/**
		 * We are using a r-tree data structure to speedup our closest-point to mesh queries.
		 * SpacialQueryCacheData contains this tree as cached object
		 */
		class SpacialQueryCacheData : CacheData {
			public:
				SpacialQueryCacheData(std::shared_ptr<const Data> a_data)
									: 	CacheData(a_data),
										m_polygon_tree(nullptr) {
				}
				/**
				 * Aliasing for readability.
				 * We are using quadratic<16> as the desired algorithm for loading data.
				 * Actually when we pass all the data to r-tree at the object creation time,
				 * it internally uses Packing algorithm for adding nodes to tree, this gives
				 * excellent performance at both r-tree creation as well as query time. More info. can be found here
				 * <a href="https://www.boost.org/doc/libs/1_63_0/libs/geometry/doc/html/geometry/spatial_indexes/introduction.html">Boost::rtree</a>
				 */
				using SpacialCachePolygonTree = rtree<PolygonIndexPair, quadratic<16>>;

				/**
				 * Computes r-tree cache for repeated closest point queries
				 */
				void compute() override;

				/**
				 * Returns unique_prt to r-tree cache. We don't want to share ownership of cache.
				 * @return unique pointer to r-tree cache
				 */
				const std::unique_ptr<SpacialCachePolygonTree>& get_polygon_tree() const {
					return m_polygon_tree;
				}

			private:
				/**
				 * Unique pointert to r-tree cache object
				 */
				std::unique_ptr<SpacialCachePolygonTree> m_polygon_tree;
		};

		/**
		 * Cached data handler of r-tree acceleration structure for PolygonMesh's closest point query.
		 * Since cache can mutate in functions like \ref PolygonMesh::closest_point
		 * we make it mutable. const_cast'ing it at requirement could be another way of doing this.
		 */
		mutable std::shared_ptr<Cache<SpacialQueryCacheData>> m_spacial_query_cache;

		/**
		 * No. of closest triangles to fetch from the acceleration structure
		 */
		const std::size_t CLOSEST_TRIANGLE_COUNT = 16;
};

} /* namespace data */

