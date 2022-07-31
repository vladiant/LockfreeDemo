#include "lockfree/exchange_buffer.hpp"

#include <utest/utest.h>

#include <memory>

namespace {

// only test the lock-free implementation of ExchangeBuffer
// TODO: typed tests for different types T (trivially copyable etc.)

using IntBuffer = lockfree::ExchangeBuffer<int>;

struct TestExchangeBuffer {
 public:
  std::unique_ptr<IntBuffer> buffer;
};

UTEST_F_SETUP(TestExchangeBuffer) {
  static_cast<void>(utest_result);
  utest_fixture->buffer = std::make_unique<IntBuffer>();
}

UTEST_F_TEARDOWN(TestExchangeBuffer) {
  static_cast<void>(utest_result);
  static_cast<void>(utest_fixture->buffer);
}

// Some tests for (single-threaded) correctness (necessary condition for
// correctness).

UTEST_F(TestExchangeBuffer, constructed_buffer_is_empty) {
  EXPECT_TRUE(utest_fixture->buffer->empty());
}

UTEST_F(TestExchangeBuffer, take_returns_nothing_if_empty) {
  auto result = utest_fixture->buffer->take();
  EXPECT_FALSE(result.has_value());
}

UTEST_F(TestExchangeBuffer, read_returns_nothing_if_empty) {
  auto result = utest_fixture->buffer->read();
  EXPECT_FALSE(result.has_value());
}

UTEST_F(TestExchangeBuffer, buffer_contains_value_after_write) {
  EXPECT_TRUE(utest_fixture->buffer->write(73));
  EXPECT_FALSE(utest_fixture->buffer->empty());
  auto result = utest_fixture->buffer->take();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, 73);
}

UTEST_F(TestExchangeBuffer, write_overwrites_previous_value) {
  EXPECT_TRUE(utest_fixture->buffer->write(73));
  EXPECT_TRUE(utest_fixture->buffer->write(37));
  auto result = utest_fixture->buffer->take();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, 37);
}

// We cannot easily test the case where a concurrent write fails
// due to lack of memory (i.e. free indices/slots)
// We cannot provoke the problem since we cannot interrupt an
// ongoing write.
// Could be done by mocking the IndexPool but then we would have to expose
// it as e.g. template parameter to allow mocking.

UTEST_F(TestExchangeBuffer, try_write_succeeds_if_empty) {
  EXPECT_TRUE(utest_fixture->buffer->try_write(73));
  auto result = utest_fixture->buffer->take();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, 73);
}

UTEST_F(TestExchangeBuffer, try_write_fails_if_not_empty) {
  EXPECT_TRUE(utest_fixture->buffer->try_write(37));
  EXPECT_FALSE(utest_fixture->buffer->empty());
  EXPECT_FALSE(utest_fixture->buffer->try_write(73));
  auto result = utest_fixture->buffer->take();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, 37);
}

UTEST_F(TestExchangeBuffer, take_removes_value) {
  EXPECT_TRUE(utest_fixture->buffer->write(73));
  auto result = utest_fixture->buffer->take();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, 73);
  EXPECT_TRUE(utest_fixture->buffer->empty());
  result = utest_fixture->buffer->take();
  EXPECT_FALSE(result.has_value());
}

UTEST_F(TestExchangeBuffer, read_does_not_remove_value) {
  EXPECT_TRUE(utest_fixture->buffer->write(73));
  auto result = utest_fixture->buffer->read();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, 73);
  EXPECT_FALSE(utest_fixture->buffer->empty());
  result = utest_fixture->buffer->read();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, 73);
}

}  // namespace