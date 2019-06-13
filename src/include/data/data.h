/*
 * Author: Pratik Mankawde
 */
#pragma once

namespace data {

/**
 * This class is responsible to represent general data. This should provide interface for memory management (CPU vs GPU),
 * locking and versioning. Data class can carry generalised definition of attributes for all the derived classes.
 * Like for Geometry class we will need positions, normals, uv etc. This data can be saved as attributes.
 */
class Data {
	public:
		Data() = default;
		virtual ~Data() = default;
};

} /* namespace data */
