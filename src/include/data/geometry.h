/*
 * Author: pratik
 */
#pragma once

#include <Eigen/Core>

#include <data/data.h>
#include <data/spacial.h>
#include <types.h>

namespace data {

class Geometry : public Data {
	public:
		Geometry();
		virtual ~Geometry();

		std::size_t get_vertex_count() {
			return m_vertices.size();
		}

		const Vec3 get_vertex_at(size_t index) const {
			if (index >= 0 && index < m_vertices.size()) {
				return m_vertices[index];
			} else {
				return Vec3();
			}
		}
		virtual Vec3 get_spacial_position(const Location& spacial_location) const = 0;

	protected:
		std::vector<Vec3> m_vertices;
};

} /* namespace data */

