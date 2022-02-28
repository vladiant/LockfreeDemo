#include "lockfree/exchange_buffer.hpp"

#include <catch2/catch.hpp>

namespace {

// only test the lock-free implementation of ExchangeBuffer
// TODO: typed tests for different types T (trivially copyable etc.)

using IntBuffer = lockfree::ExchangeBuffer<int>;

class TestExchangeBuffer {
 public:
  IntBuffer buffer;
};

// Some tests for (single-threaded) correctness (necessary condition for
// correctness).

TEST_CASE_METHOD(TestExchangeBuffer, "constructed_buffer_is_empty") {
  CHECK(buffer.empty());
}

TEST_CASE_METHOD(TestExchangeBuffer, "take_returns_nothing_if_empty") {
  auto result = buffer.take();
  CHECK_FALSE(result.has_value());
}

TEST_CASE_METHOD(TestExchangeBuffer, "read_returns_nothing_if_empty") {
  auto result = buffer.read();
  CHECK_FALSE(result.has_value());
}

TEST_CASE_METHOD(TestExchangeBuffer, "buffer_contains_value_after_write") {
  CHECK(buffer.write(73));
  CHECK_FALSE(buffer.empty());
  auto result = buffer.take();
  REQUIRE(result.has_value());
  CHECK(*result == 73);
}

TEST_CASE_METHOD(TestExchangeBuffer, "write_overwrites_previous_value") {
  CHECK(buffer.write(73));
  CHECK(buffer.write(37));
  auto result = buffer.take();
  REQUIRE(result.has_value());
  CHECK(*result == 37);
}

// We cannot easily test the case where a concurrent write fails
// due to lack of memory (i.e. free indices/slots)
// We cannot provoke the problem since we cannot interrupt an
// ongoing write.
// Could be done by mocking the IndexPool but then we would have to expose
// it as e.g. template parameter to allow mocking.

TEST_CASE_METHOD(TestExchangeBuffer, "try_write_succeeds_if_empty") {
  CHECK(buffer.try_write(73));
  auto result = buffer.take();
  REQUIRE(result.has_value());
  CHECK(*result == 73);
}

TEST_CASE_METHOD(TestExchangeBuffer, "try_write_fails_if_not_empty") {
  CHECK(buffer.try_write(37));
  CHECK_FALSE(buffer.empty());
  CHECK_FALSE(buffer.try_write(73));
  auto result = buffer.take();
  REQUIRE(result.has_value());
  CHECK(*result == 37);
}

TEST_CASE_METHOD(TestExchangeBuffer, "take_removes_value") {
  CHECK(buffer.write(73));
  auto result = buffer.take();
  REQUIRE(result.has_value());
  CHECK(*result == 73);
  CHECK(buffer.empty());
  result = buffer.take();
  CHECK_FALSE(result.has_value());
}

TEST_CASE_METHOD(TestExchangeBuffer, "read_does_not_remove_value") {
  CHECK(buffer.write(73));
  auto result = buffer.read();
  REQUIRE(result.has_value());
  CHECK(*result == 73);
  CHECK_FALSE(buffer.empty());
  result = buffer.read();
  REQUIRE(result.has_value());
  CHECK(*result == 73);
}

}  // namespace