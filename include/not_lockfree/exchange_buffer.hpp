#pragma once

#include <atomic>
#include <optional>
#include <type_traits>

#include "lockfree/index_pool.hpp"
#include "lockfree/storage.hpp"

namespace not_lockfree {

// NB: not lock-free due to std::optional using dynamic memory
// a static memory optional can be found e.g. in
// https://github.com/eclipse-iceoryx/iceoryx/blob/master/iceoryx_hoofs/include/iceoryx_hoofs/cxx/optional.hpp
template <class T, uint32_t C = 8>
class ExchangeBuffer {
 private:
  using storage_t = lockfree::Storage<T, C>;
  using indexpool_t = lockfree::IndexPool<C>;

  using index_t = typename indexpool_t::index_t;

  static constexpr index_t kNoData = C;

  std::atomic<index_t> m_index{kNoData};
  indexpool_t m_indices;
  storage_t m_storage;

 public:
  bool write(const T &value) {
    auto maybeIndex = m_indices.get();
    if (!maybeIndex) {
      return false;  // no index
    }
    auto index = maybeIndex.value();
    // will succeed since we the exclusive ownership of index
    m_storage.store_at(value, index);

    auto oldIndex = m_index.exchange(index);
    if (oldIndex != kNoData) {
      free(oldIndex);
    }
    return true;
  }

  bool try_write(const T &value) {
    auto maybeIndex = m_indices.get();
    if (!maybeIndex) {
      return false;  // no index
    }
    auto index = maybeIndex.value();
    m_storage.store_at(value, index);

    index_t expected = kNoData;
    if (!m_index.compare_exchange_strong(expected, index)) {
      free(index);
      return false;
    }
    return true;
  }

  std::optional<T> take() {
    auto index = m_index.exchange(kNoData);
    if (index == kNoData) {
      return std::nullopt;
    }
    auto ret = std::optional<T>(std::move(m_storage[index]));
    free(index);
    return ret;
  }

  std::optional<T> read1() {
    auto index = m_index.load();
    if (index == kNoData) {
      return std::nullopt;
    }
    // cannot read while it could change concurrently
    auto ret = std::optional<T>(m_storage[index]);
    return ret;
  }

  std::optional<T> read() {
    auto index = m_index.load();

    while (index != kNoData) {
      auto ret = std::optional<T>(m_storage[index]);

      if (m_index.compare_exchange_strong(index, index)) {
        return ret;
      }
    }
    return std::nullopt;
  }

  bool empty() { return m_index.load() == kNoData; }

 private:
  void free(index_t index) {
    m_storage.free(index);
    m_indices.free(index);
  }
};

}  // namespace not_lockfree
