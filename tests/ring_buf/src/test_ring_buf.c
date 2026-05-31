/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	/* Push a value, call rb_init(4) again, then verify the buffer is empty
	 * and count is 0.
	 */
	rb_push(11);
	rb_init(4);
	zassert_true(rb_is_empty(), "Buffer must be empty after re-init");
	zassert_equal(rb_count(), 0, "Count must be 0 after re-init");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	/* rb_push(42), rb_pop(&v) -> v == 42, buffer empty after. */
	int v;

	zassert_equal(rb_push(42), 0, "push(42) should succeed");
	zassert_equal(rb_pop(&v), 0, "pop should succeed");
	zassert_equal(v, 42, "Popped value should be 42");
	zassert_true(rb_is_empty(), "Buffer should be empty after pop");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	/* rb_push(1), rb_push(2), rb_push(3); pop three times and verify the
	 * values come out as 1, 2, 3 in that order.
	 */
	int v;

	zassert_equal(rb_push(1), 0, "push(1) should succeed");
	zassert_equal(rb_push(2), 0, "push(2) should succeed");
	zassert_equal(rb_push(3), 0, "push(3) should succeed");

	rb_pop(&v);
	zassert_equal(v, 1, "First pop should be 1");
	rb_pop(&v);
	zassert_equal(v, 2, "Second pop should be 2");
	rb_pop(&v);
	zassert_equal(v, 3, "Third pop should be 3");

	zassert_true(rb_is_empty(), "Buffer should be empty after all pops");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	/* Fill the buffer to its capacity of 4, then push one more value
	 * -> -ENOSPC.
	 */
	zassert_equal(rb_push(1), 0, "push(1) should succeed");
	zassert_equal(rb_push(2), 0, "push(2) should succeed");
	zassert_equal(rb_push(3), 0, "push(3) should succeed");
	zassert_equal(rb_push(4), 0, "push(4) should succeed");

	zassert_true(rb_is_full(), "Buffer should be full");
	zassert_equal(rb_push(11), -ENOSPC, "Push to full buffer must return -ENOSPC");
	zassert_equal(rb_count(), 4, "Count must stay at 4 after rejected push");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	/* rb_push(7); rb_peek(&v) -> v == 7; rb_peek(&v) again -> v == 7;
	 * rb_count() still == 1.
	 */
	int v;

	rb_push(7);

	zassert_equal(rb_peek(&v), 0, "First peek should succeed");
	zassert_equal(v, 7, "First peek should return 7");

	v = 0;
	zassert_equal(rb_peek(&v), 0, "Second peek should succeed");
	zassert_equal(v, 7, "Second peek should still return 7");

	zassert_equal(rb_count(), 1, "Count must stay 1 — peek must not consume");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	/* rb_pop(NULL) -> -EINVAL. */
	zassert_equal(rb_pop(NULL), -EINVAL, "pop(NULL) must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	/* Push 4 values -> rb_is_full() == true, rb_count() == 4. */
	zassert_equal(rb_push(10), 0, "push(10) should succeed");
	zassert_equal(rb_push(20), 0, "push(20) should succeed");
	zassert_equal(rb_push(30), 0, "push(30) should succeed");
	zassert_equal(rb_push(40), 0, "push(40) should succeed");

	zassert_true(rb_is_full(), "Buffer should be full after 4 pushes");
	zassert_equal(rb_count(), 4, "Count should be 4");
}

ZTEST(ring_buf_boundaries, test_init_zero_capacity_returns_einval)
{
	/* rb_init(0) -> -EINVAL (capacity must be at least 1). */
	zassert_equal(rb_init(0), -EINVAL, "rb_init(0) must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_init_over_max_capacity_returns_einval)
{
	/* rb_init(capacity > RING_BUF_MAX_CAPACITY) -> -EINVAL. */
	zassert_equal(rb_init(RING_BUF_MAX_CAPACITY + 1), -EINVAL,
		      "rb_init above max capacity must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_pop_empty_returns_enodata)
{
	/* rb_pop(&v) on an empty buffer -> -ENODATA. */
	int v;

	zassert_equal(rb_pop(&v), -ENODATA, "pop on empty buffer must return -ENODATA");
}

ZTEST(ring_buf_boundaries, test_peek_null_returns_einval)
{
	/* rb_peek(NULL) -> -EINVAL. */
	zassert_equal(rb_peek(NULL), -EINVAL, "peek(NULL) must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_peek_empty_returns_enodata)
{
	/* rb_peek(&v) on an empty buffer -> -ENODATA. */
	int v;

	zassert_equal(rb_peek(&v), -ENODATA, "peek on empty buffer must return -ENODATA");
}
