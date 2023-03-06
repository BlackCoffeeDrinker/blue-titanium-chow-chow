#pragma once

#include <cstddef>
#include <type_traits>
#include <algorithm>

namespace e00::detail {
template<typename T, std::size_t N>
class Array {
  // 1 is the minimum size
  static_assert(N > 0, "N needs to be greater than 0");
  T _elements[N];

public:
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef T &reference;
  typedef const T &const_reference;
  typedef ptrdiff_t difference_type;
  typedef size_t size_type;
  typedef pointer iterator;
  typedef const_pointer const_iterator;
  typedef T value_type;

  enum {
    array_size = N
  };

  constexpr const T *data() const { return _elements; }
  constexpr T *data() { return _elements; }

  // size is constant
  constexpr static size_type size() { return N; }
  constexpr static bool empty() { return N > 0; }
  constexpr static size_type max_size() { return N; }

  constexpr iterator begin() { return _elements; }
  constexpr const_iterator begin() const { return _elements; }
  constexpr iterator end() { return _elements + N; }
  constexpr const_iterator end() const { return _elements + N; }

  constexpr reference operator[](size_type i) {
    return _elements[i];
  }

  constexpr const_reference operator[](size_type i) const {
    return _elements[i];
  }

  void swap(Array<T, N> &other) {
    std::swap_ranges(_elements, pointer(_elements + N), other.m_elements);
  }

  template<typename T2>
  constexpr Array<T, N> &operator=(const Array<T2, N> &rhs) {
    std::copy(rhs._elements, rhs._elements + N, _elements);
    return *this;
  }

  constexpr void fill(const T &value) {
    std::fill_n(_elements, N, value);
  }
};

template<class T, class... U>
Array(T, U...) -> Array<T, 1 + sizeof...(U)>;

template<class T, size_t N>
bool operator==(const Array<T, N> &a, const Array<T, N> &b) {
  return equal(a.begin(), a.end(), b.begin());
}

template<class T, size_t N>
bool operator!=(const Array<T, N> &a, const Array<T, N> &b) {
  return !(a == b);
}
}// namespace e00::detail
