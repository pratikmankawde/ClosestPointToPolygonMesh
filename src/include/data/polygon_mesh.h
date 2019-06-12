/*
 * Author: pratik
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
 * This class represents a polygon mesh.
 * Vertices are stored directly into a vector in Geometry class. Topology is stored separately here.
 * Each entry in topology is a collection of vertices of a polygon. We are assuming a triangle based mesh here.
 * We can have generalised version of polygon mesh as this one's parent class and then specialised triangle based
 * mesh implemented here. We are keeping this simple for now.
 */
class PolygonMesh : public Geometry, public std::enable_shared_from_this<PolygonMesh> {
	public:

		PolygonMesh()
					: Geometry()
					, m_spacial_query_cache(std::make_shared<Cache<SpacialQueryCacheData>>()) {};

		virtual ~PolygonMesh();

		using Polygon = Vec3ui;
		using PolygonIndexPair = std::pair<BoundingVolume, size_t>;

		/**
		 * @param a_vertex_locations 	Vertex locations
		 * @param a_topology			Topology of polygon mesh. Ex.: {{1,2}, {2,3,4,5}, {3,6,7,4}}
		 * @return
		 */
		bool build_mesh(const std::vector<Vec3>& a_vertex_locations, const std::vector<Polygon>& a_topology);

		Location closest_point(const Vec3& a_query_point);

		Vec3 get_spacial_position(const Location& spacial_location) const override;

		size_t get_polygon_count() {
			return m_topology.size();
		}

		inline const Polygon get_polygon_at(size_t index) const {
			if (index >= 0 && index < m_topology.size()) {
				return m_topology[index];
			} else {
				return Polygon();
			}
		}

		// factory function that creates shared pttr for Polygonmesh
		static std::shared_ptr<PolygonMesh> create() {
			return std::shared_ptr<PolygonMesh>(new PolygonMesh());
		}

	private:

		/**
		 * We are assuming mesh is triangle based, hence we will have only 3 vertices defining a polygon.
		 * In cases where polygon has more than 3 vertices we can handle it in two ways:
		 * 1. We can triangulate the mesh
		 * 2. We can define topology as sm_spacial_query_cachetd::vector<block>, where 'block' datatype can simply be
		 *	  described as std::vector<std::size_t>. Alternatively we can implement it as a block array data structure,
		 *	  may be stack allocated, if that can give us some performance improvement.
		 * For simplicity we will only handle a triangle based mesh for now. We are using general data type
		 * we defined in types.h Vec3ui to store index of vertices in the polygon.
		 */
		std::vector<Polygon> m_topology;

		/**
		 * We are using a r-tree data structure to speedup out closest-point to mesh queries.
		 */
		class SpacialQueryCacheData : CacheData {
			public:
				SpacialQueryCacheData(std::shared_ptr<Data> a_data)
									: 	CacheData(a_data),
										m_polygon_tree(nullptr) {
				}
				using SpacialCachePolygonTree = rtree<PolygonIndexPair, quadratic<16>>;

				void compute() override;
				const std::shared_ptr<SpacialCachePolygonTree> get_polygon_tree() const {
					return m_polygon_tree;
				}

			private:
				std::shared_ptr<SpacialCachePolygonTree> m_polygon_tree;
		};

		std::shared_ptr<Cache<SpacialQueryCacheData>> m_spacial_query_cache;
};

} /* namespace data */

