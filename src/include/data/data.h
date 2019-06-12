/*
 * Author: pratik
 */
#pragma once

namespace data {

/**
 * This class is responsible to represent general data.
 * This should provide interface for memory management (CPU vs GPU),
 * locking and versioning.
 */
class Data {
	public:
		Data();
		virtual ~Data();
};

} /* namespace data */
