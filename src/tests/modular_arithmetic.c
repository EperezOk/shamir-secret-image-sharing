#include <criterion/criterion.h>

#include "../include/modular_arithmetic.h"

// Designed to work with prime P=251

uint32_t a = 127;
uint32_t b = 210;

Test(modular_arithmetic, addition) {
    cr_assert_eq(zAdd(a, b), 86);
}

Test(modular_arithmetic, subtraction) {
    cr_assert_eq(zSub(a, b), 168);
}

Test(modular_arithmetic, multiplication) {
    cr_assert_eq(zMul(a, b), 64);
}

Test(modular_arithmetic, division) {
    cr_assert_eq(zDiv(a, b), 52);
}

Test(modular_arithmetic, positive) {
    cr_assert_eq(zPos(-4), 247);
}

Test(modular_arithmetic, inverse) {
    cr_assert_eq(inv(a), 168);
}
