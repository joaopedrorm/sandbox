#define mu_assert_eq(expected, actual) { \
    int a = (expected); \
    int b = (actual); \
    do { \
        if ((a) != (b)) { \
            sprintf(minunit_msg, "FAILURE %s:%d expected %d, got %u", __FILE__, __LINE__, a, b); \
            return minunit_msg; \
        }; \
    } while(0); \
}
