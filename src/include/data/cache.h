/*
 * Author: Pratik Mankawde
 */
#pragma once

#include <memory>
#include <data/data.h>
#include <types.h>

namespace data {

// TODO: May be we should implement this following RAII in case we decide to serialize cache at some point
// Not needed for now.
/**
 * CacheData is base class for cache in general. We have kept it simple for now.
 * It could contain general logic for memory management on CPU/GPU and possibly serialization
 */
class CacheData : public Data {
	public:
		/**
		 * Standard constructor
		 */
		CacheData(std::shared_ptr<const Data> a_data)
				: 	Data(),
					m_data(a_data) {
		};

		~CacheData() = default;

		/**
		 * All subclasses must implement this function to construct their required cache data.
		 * We have m_data as base data for constructing cache from
		 */
		virtual void compute() = 0;

	protected:
		/**
		 * m_data will provide necessary data for constructing cache
		 */
		std::shared_ptr<const Data> m_data;
};

/**
 * Abstract Base class representing data caches.
 * Cache creation and handing will be taken care by sub classes implementing this class
 */
template<typename CacheDataType>
class Cache {
	public:
		Cache() = default;

		virtual ~Cache() = default;

		/**
		 * Function to invalidate cache
		 */
		virtual void invalidate() {
			m_cache_data = nullptr;
		}
		/**
		 * Checks if the cache is valid. Again cache validity can be tested in various other ways
		 * then just checking the pointer validity. Subclasses can implement it accordingly.
		 */
		virtual bool is_valid() {
			return (static_cast<bool>(m_cache_data));
		}

		/**
		 * This function creates a cache data object and populate cache.
		 * It has been made virtual so that sub-classes can implement their own behaviour if desired
		 * @param a_data Data used for generating cache
		 * @return Ref. to unique_pointer to cached data.
		 */
		virtual const std::unique_ptr<CacheDataType>& get_cached_data(std::shared_ptr<const Data> a_data) {
			if (!static_cast<bool>(m_cache_data)) {
				m_cache_data = std::unique_ptr<CacheDataType>(new CacheDataType(a_data));
				m_cache_data->compute();
			}
			return m_cache_data;
		}
	private:
		// TODO When used under parallel access, there should be a mutex
		// defined here for synchronisation
		// mutable tbb::...::mutex;
		/**
		 *	Unique pointer to the cached data We do not want the ownership to be shared
		 */
		std::unique_ptr<CacheDataType> m_cache_data;
};

} /* namespace data */

