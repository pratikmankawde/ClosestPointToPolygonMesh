/*
 * Author: Pratik
 */
#pragma once

#include <memory>
#include <data/data.h>

namespace data {

// TODO: May be we should implement this following RAII in case we are serialising cache. Not needed for now.
class CacheData : public Data {
	public:
		CacheData(std::shared_ptr<Data> a_data)
				: 	Data(),
					m_data(a_data) {};

		~CacheData() = default;

		virtual void compute() = 0;

	protected:
		std::shared_ptr<Data> m_data;
};

/**
 * Abstract Base class representing data caches
 * Cache creation and handing will be taken care by sub classes implementing this class
 *
 */
template<typename CacheDataType>
class Cache {
	public:
		Cache() = default;

		virtual ~Cache() = default;

		void invalidate() {
			m_cache_data = std::shared_ptr<CacheDataType>();
		}

		bool is_valid() {
			return (static_cast<bool>(m_cache_data));
		}

		virtual const std::shared_ptr<CacheDataType> get_cached_data(std::shared_ptr<Data> a_data) {
			if (!static_cast<bool>(m_cache_data)) {
				m_cache_data = std::make_shared<CacheDataType>(a_data);
				m_cache_data->compute();
			}
			return m_cache_data;
		}
	private:
		// TODO When used under parallel access, there should be a mutex
		// defined here for synchronisation
		// mutable tbb::...::mutex;
		std::shared_ptr<CacheDataType> m_cache_data;
};

} /* namespace data */

