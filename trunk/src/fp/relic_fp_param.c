/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007-2011 RELIC Authors
 *
 * This file is part of RELIC. RELIC is legal property of its developers,
 * whose names are not listed here. Please refer to the COPYRIGHT file
 * for contact information.
 *
 * RELIC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * RELIC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RELIC. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Implementation of the prime field modulus manipulation.
 *
 * @version $Id$
 * @ingroup fb
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "relic_core.h"
#include "relic_conf.h"
#include "relic_dv.h"
#include "relic_fp.h"
#include "relic_fp_low.h"
#include "relic_error.h"

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

/**
 * Current configured prime field identifier.
 */
static int param_id;

/**
 * Maximum number of powers of 2 used to describe specia form moduli.
 */
#define MAX_EXPS		10

/**
 * Non-zero bits of special form prime.
 */
static int spars[MAX_EXPS + 1] = { 0 };

/**
 * Number of bits of special form parameter.
 */
static int spars_len = 0;

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

int fp_param_get(void) {
	return param_id;
}

void fp_param_get_var(bn_t x) {
	bn_t a;

	bn_null(a);

	TRY {
		bn_new(a);

		switch (param_id) {
			case BN_158:
				/* x = 4000000031. */
				bn_set_2b(x, 38);
				bn_add_dig(x, x, 0x31);
				break;
			case BN_254:
				/* x = -4080000000000001. */
				bn_set_2b(x, 62);
				bn_set_2b(a, 55);
				bn_add(x, x, a);
				bn_add_dig(x, x, 1);
				bn_neg(x, x);
				break;
			case BN_256:
				/* x = -600000000000219B. */
				bn_set_2b(x, 62);
				bn_set_2b(a, 61);
				bn_add(x, x, a);
				bn_set_dig(a, 0x21);
				bn_lsh(a, a, 8);
				bn_add(x, x, a);
				bn_add_dig(x, x, 0x9B);
				bn_neg(x, x);
				break;
			case BN_638:
				/* x = 2^158 - 2^128 - 2^68 + 1. */
				bn_set_2b(x, 158);
				bn_set_2b(a, 128);
				bn_sub(x, x, a);
				bn_set_2b(a, 68);
				bn_sub(x, x, a);
				bn_add_dig(x, x, 1);
				break;
			default:
				THROW(ERR_INVALID);
				break;
		}
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		bn_free(a);
	}
}

int *fp_param_get_sps(int *len) {
	bn_t a;

	bn_null(a);

	TRY {
		bn_new(a);

		spars_len = 0;

		switch (param_id) {
			case BN_158:
			case BN_254:
			case BN_256:
				fp_param_get_var(a);
				if (bn_sign(a) == BN_NEG) {
					bn_neg(a, a);
				}
				spars_len = bn_ham(a);
				for (int i = 0, j = 0; j < bn_bits(a); j++) {
					if (bn_test_bit(a, j)) {
						spars[i++] = j;
					}
				}
				break;
			case BN_638:
				spars_len = 4;
				spars[0] = 0;
				spars[1] = -68;
				spars[2] = -128;
				spars[3] = 158;
				break;
			default:
				THROW(ERR_INVALID);
				break;
		}

		if (spars_len > 0 && spars_len < MAX_EXPS ) {
			if (len != NULL) {
				*len = spars_len;
			}
			return spars;
		} else {
			if (len != NULL) {
				*len = 0;
			}
			return NULL;
		}
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		bn_free(a);
	}

	return NULL;
}

void fp_param_set(int param) {
	bn_t t0, t1, p;
	int f[10] = { 0 };
	int generated = 0;

	/* Suppress possible unused parameter warning. */
	(void) f;

	TRY {
		bn_new(t0);
		bn_new(t1);
		bn_new(p);

		param_id = param;

		switch (param) {
#if FP_PRIME == 158
			case BN_158:
				/* x = 4000000031. */
				fp_param_get_var(t0);
				/* p = 36 * x^4 + 36 * x^3 + 24 * x^2 + 6 * x + 1. */
				bn_set_dig(p, 1);
				bn_mul_dig(t1, t0, 6);
				bn_add(p, p, t1);
				bn_mul(t1, t0, t0);
				bn_mul_dig(t1, t1, 24);
				bn_add(p, p, t1);
				bn_mul(t1, t0, t0);
				bn_mul(t1, t1, t0);
				bn_mul_dig(t1, t1, 36);
				bn_add(p, p, t1);
				bn_mul(t0, t0, t0);
				bn_mul(t1, t0, t0);
				bn_mul_dig(t1, t1, 36);
				bn_add(p, p, t1);
				fp_prime_set_dense(p);
				break;
#elif FP_PRIME == 160
			case SECG_160:
				/* p = 2^160 - 2^31 + 1. */
				f[0] = -1;
				f[1] = -31;
				fp_prime_set_pmers(f, 2);
				break;
			case SECG_160D:
				/* p = 2^160 - 2^32 - 2^14 - 2^12 - 2^9 - 2^8 - 2^7 - 2^3 - 2^2 - 1.*/
				f[0] = -1;
				f[1] = -2;
				f[2] = -3;
				f[3] = -7;
				f[4] = -8;
				f[5] = -9;
				f[6] = -12;
				f[7] = -14;
				f[8] = -32;
				fp_prime_set_pmers(f, 9);
				break;
#elif FP_PRIME == 192
			case NIST_192:
				/* p = 2^192 - 2^64 - 1. */
				f[0] = -1;
				f[1] = -64;
				fp_prime_set_pmers(f, 2);
				break;
			case SECG_192:
				/* p = 2^192 - 2^32 - 2^12 - 2^8 - 2^7 - 2^6 - 2^3 - 1.*/
				f[0] = -1;
				f[1] = -3;
				f[2] = -6;
				f[3] = -7;
				f[4] = -8;
				f[5] = -12;
				f[6] = -32;
				fp_prime_set_pmers(f, 7);
				break;
#elif FP_PRIME == 224
			case NIST_224:
				/* p = 2^224 - 2^96 + 1. */
				f[0] = 1;
				f[1] = -96;
				fp_prime_set_pmers(f, 2);
				break;
			case SECG_224:
				/* p = 2^224 - 2^32 - 2^12 - 2^11 - 2^9 - 2^7 - 2^4 - 2 - 1.*/
				f[0] = -1;
				f[1] = -1;
				f[2] = -4;
				f[3] = -7;
				f[4] = -9;
				f[5] = -11;
				f[6] = -12;
				f[7] = -32;
				fp_prime_set_pmers(f, 8);
				break;
#elif FP_PRIME == 254
			case BN_254:
				/* x = -4080000000000001. */
				fp_param_get_var(t0);
				/* p = 36 * x^4 + 36 * x^3 + 24 * x^2 + 6 * x + 1. */
				bn_set_dig(p, 1);
				bn_mul_dig(t1, t0, 6);
				bn_add(p, p, t1);
				bn_mul(t1, t0, t0);
				bn_mul_dig(t1, t1, 24);
				bn_add(p, p, t1);
				bn_mul(t1, t0, t0);
				bn_mul(t1, t1, t0);
				bn_mul_dig(t1, t1, 36);
				bn_add(p, p, t1);
				bn_mul(t0, t0, t0);
				bn_mul(t1, t0, t0);
				bn_mul_dig(t1, t1, 36);
				bn_add(p, p, t1);
				fp_prime_set_dense(p);
				break;
#elif FP_PRIME == 256
			case NIST_256:
				/* p = 2^256 - 2^224 + 2^192 + 2^96 - 1. */
				f[0] = -1;
				f[1] = 96;
				f[2] = 192;
				f[3] = -224;
				fp_prime_set_pmers(f, 4);
				break;
			case SECG_256:
				/* p = 2^256 - 2^32 - 2^9 - 2^8 - 2^7 - 2^6 - 2^4 - 1. */
				f[0] = -1;
				f[1] = -4;
				f[2] = -6;
				f[3] = -7;
				f[4] = -8;
				f[5] = -9;
				f[6] = -32;
				fp_prime_set_pmers(f, 7);
				break;
			case BN_256:
				/* x = 6000000000001F2D. */
				fp_param_get_var(t0);
				/* p = 36 * x^4 + 36 * x^3 + 24 * x^2 + 6 * x + 1. */
				bn_set_dig(p, 1);
				bn_mul_dig(t1, t0, 6);
				bn_add(p, p, t1);
				bn_mul(t1, t0, t0);
				bn_mul_dig(t1, t1, 24);
				bn_add(p, p, t1);
				bn_mul(t1, t0, t0);
				bn_mul(t1, t1, t0);
				bn_mul_dig(t1, t1, 36);
				bn_add(p, p, t1);
				bn_mul(t0, t0, t0);
				bn_mul(t1, t0, t0);
				bn_mul_dig(t1, t1, 36);
				bn_add(p, p, t1);
				fp_prime_set_dense(p);
				break;
#elif FP_PRIME == 384
			case NIST_384:
				/* p = 2^384 - 2^128 - 2^96 + 2^32 - 1. */
				f[0] = -1;
				f[1] = 32;
				f[2] = -96;
				f[3] = -128;
				fp_prime_set_pmers(f, 4);
				break;
#elif FP_PRIME == 521
			case NIST_521:
				/* p = 2^521 - 1. */
				f[0] = -1;
				fp_prime_set_pmers(f, 1);
				break;
#elif FP_PRIME == 638
			case BN_638:
				fp_param_get_var(t0);
				/* p = 36 * x^4 + 36 * x^3 + 24 * x^2 + 6 * x + 1. */
				bn_set_dig(p, 1);
				bn_mul_dig(t1, t0, 6);
				bn_add(p, p, t1);
				bn_mul(t1, t0, t0);
				bn_mul_dig(t1, t1, 24);
				bn_add(p, p, t1);
				bn_mul(t1, t0, t0);
				bn_mul(t1, t1, t0);
				bn_mul_dig(t1, t1, 36);
				bn_add(p, p, t1);
				bn_mul(t0, t0, t0);
				bn_mul(t1, t0, t0);
				bn_mul_dig(t1, t1, 36);
				bn_add(p, p, t1);
				fp_prime_set_dense(p);
				break;
#else
			default:
				bn_gen_prime(p, FP_BITS);
				fp_prime_set_dense(p);
				generated = 1;
				break;
#endif
		}

		if (generated) {
			param_id = 0;
		}
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		bn_free(t0);
		bn_free(t1);
		bn_free(p);
	}
}

int fp_param_set_any(void) {
#if FP_PRIME == 158
	fp_param_set(BN_158);
#elif FP_PRIME == 160
	fp_param_set(SECG_160);
#elif FP_PRIME == 192
	fp_param_set(NIST_192);
#elif FP_PRIME == 224
	fp_param_set(NIST_224);
#elif FP_PRIME == 254
	fp_param_set(BN_254);
#elif FP_PRIME == 256
#ifdef FP_PMERS
	fp_param_set(SECG_256);
#else
	fp_param_set(BN_256);
#endif
#elif FP_PRIME == 384
	fp_param_set(NIST_384);
#elif FP_PRIME == 521
	fp_param_set(NIST_521);
#elif FP_PRIME == 638
	fp_param_set(BN_638);
#else
	return fp_param_set_any_dense();
#endif
	return STS_OK;
}

int fp_param_set_any_dense() {
	bn_t modulus;
	int result = STS_OK;

	bn_null(modulus);

	TRY {
		bn_new(modulus);
		bn_gen_prime(modulus, FP_BITS);
		if (!bn_is_prime(modulus)) {
			result = STS_ERR;
		} else {
			fp_prime_set_dense(modulus);
		}
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		bn_free(modulus);
	}
	return result;
}

int fp_param_set_any_pmers(void) {
#if FP_PRIME == 160
	fp_param_set(SECG_160);
#elif FP_PRIME == 192
	fp_param_set(NIST_192);
#elif FP_PRIME == 224
	fp_param_set(NIST_224);
#elif FP_PRIME == 256
	fp_param_set(NIST_256);
#elif FP_PRIME == 384
	fp_param_set(NIST_384);
#elif FP_PRIME == 521
	fp_param_set(NIST_521);
#else
	return STS_ERR;
#endif
	return STS_OK;
}

int fp_param_set_any_tower() {
#if FP_PRIME == 158
	fp_param_set(BN_158);
#elif FP_PRIME == 254
	fp_param_set(BN_254);
#elif FP_PRIME == 256
	fp_param_set(BN_256);
#elif FP_PRIME == 638
	fp_param_set(BN_638);
#else
	do {
		fp_param_set_any_dense();
	} while (fp_prime_get_mod5() == 1 || fp_prime_get_mod5() == 4 ||
			fp_prime_get_mod8() == 1);
#endif
	if (fp_prime_get_mod5() == 1 || fp_prime_get_mod5() == 4 ||
			fp_prime_get_mod8() == 1) {
		return STS_ERR;
	}
	return STS_OK;
}

void fp_param_print(void) {
	util_print_banner("Prime modulus:", 0);
	util_print("   ");
	fp_print(fp_prime_get());
}