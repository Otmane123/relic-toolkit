/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007-2012 RELIC Authors
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

/**f
 * @file
 *
 * Implementation of the prime elliptic curve utilities.
 *
 * @version $Id$
 * @ingroup ep
 */

#include "relic_core.h"

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

/**
 * Detects an optimization based on the curve coefficients.
 *
 * @param opt		- the resulting optimization.
 * @param a			- the curve coefficient.
 */
static void detect_opt(int *opt, fp_t a) {
	fp_t t;

	fp_null(t);

	TRY {
		fp_new(t);
		fp_prime_conv_dig(t, 3);
		fp_neg(t, t);

		if (fp_cmp(a, t) == CMP_EQ) {
			*opt = OPT_MINUS3;
		} else {
			if (fp_is_zero(a)) {
				*opt = OPT_ZERO;
			} else {
				fp_set_dig(t, 1);
				if (fp_cmp_dig(a, 1) == CMP_EQ) {
					*opt = OPT_ONE;
				} else {
					if (fp_cmp_dig(a, 2) == CMP_EQ) {
						*opt = OPT_TWO;
					} else {
						if (fp_bits(a) <= FP_DIGIT) {
							*opt = OPT_DIGIT;
						} else {
							*opt = OPT_NONE;
						}
					}
				}
			}
		}
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp_free(t);
	}
}

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void ep_curve_init(void) {
	ctx_t *ctx = core_get();
#ifdef EP_PRECO
	for (int i = 0; i < EP_TABLE; i++) {
		ctx->ep_ptr[i] = &(ctx->ep_pre[i]);
	}
#endif
#if ALLOC == STATIC
	fp_new(ctx->ep_g.x);
	fp_new(ctx->ep_g.y);
	fp_new(ctx->ep_g.z);
#ifdef EP_PRECO
	for (int i = 0; i < EP_TABLE; i++) {
		fp_new(ctx->ep_pre[i].x);
		fp_new(ctx->ep_pre[i].y);
		fp_new(ctx->ep_pre[i].z);
	}
#endif
#endif
	ep_set_infty(&ctx->ep_g);
	bn_init(&ctx->ep_r, FP_DIGS);
	bn_init(&ctx->ep_h, FP_DIGS);
#if defined(EP_KBLTZ) && (EP_MUL == LWNAF || EP_FIX == COMBS || EP_FIX == LWNAF || !defined(STRIP))
	for (int i = 0; i < 3; i++) {
		bn_init(&(ctx->v1[i]), FP_DIGS);
		bn_init(&(ctx->v2[i]), FP_DIGS);
	}
#endif
}

void ep_curve_clean(void) {
	ctx_t *ctx = core_get();
#if ALLOC == STATIC
	fp_free(ctx->ep_g.x);
	fp_free(ctx->ep_g.y);
	fp_free(ctx->ep_g.z);
#ifdef EP_PRECO
	for (int i = 0; i < EP_TABLE; i++) {
		fp_free(ctx->ep_pre[i].x);
		fp_free(ctx->ep_pre[i].y);
		fp_free(ctx->ep_pre[i].z);
	}
#endif
#endif
	bn_clean(&ctx->ep_r);
	bn_clean(&ctx->ep_h);
#if defined(EP_KBLTZ) && (EP_MUL == LWNAF || EP_FIX == LWNAF || !defined(STRIP))
	for (int i = 0; i < 3; i++) {
		bn_clean(&(ctx->v1[i]));
		bn_clean(&(ctx->v2[i]));
	}
#endif
}

dig_t *ep_curve_get_b() {
	return core_get()->ep_b;
}

dig_t *ep_curve_get_a() {
	return core_get()->ep_a;
}

#if defined(EP_KBLTZ) && (EP_MUL == LWNAF || EP_FIX == COMBS || EP_FIX == LWNAF || EP_SIM == INTER || !defined(STRIP))

dig_t *ep_curve_get_beta() {
	return core_get()->beta;
}

void ep_curve_get_v1(bn_t v[]) {
	ctx_t *ctx = core_get();
	for (int i = 0; i < 3; i++) {
		bn_copy(v[i], &(ctx->v1[i]));
	}
}

void ep_curve_get_v2(bn_t v[]) {
	ctx_t *ctx = core_get();
	for (int i = 0; i < 3; i++) {
		bn_copy(v[i], &(ctx->v2[i]));
	}
}

#endif

int ep_curve_opt_a() {
	return core_get()->ep_opt_a;
}

int ep_curve_opt_b() {
	return core_get()->ep_opt_b;
}

int ep_curve_is_kbltz() {
	return core_get()->ep_is_kbltz;
}

int ep_curve_is_super() {
	return core_get()->ep_is_super;
}

void ep_curve_get_gen(ep_t g) {
	ep_copy(g, &core_get()->ep_g);
}

void ep_curve_get_ord(bn_t n) {
	bn_copy(n, &core_get()->ep_r);
}

void ep_curve_get_cof(bn_t h) {
	bn_copy(h, &core_get()->ep_h);
}

ep_t *ep_curve_get_tab() {
#if defined(EP_PRECO)

	/* Return a meaningful pointer. */
#if ALLOC == AUTO
	return (ep_t *)*core_get()->ep_ptr;
#else
	return core_get()->ep_ptr;
#endif

#else
	/* Return a null pointer. */
	return NULL;
#endif
}

#if defined(EP_ORDIN)

void ep_curve_set_ordin(fp_t a, fp_t b, ep_t g, bn_t r, bn_t h) {
	ctx_t *ctx = core_get();
	ctx->ep_is_kbltz = 0;

	fp_copy(ctx->ep_a, a);
	fp_copy(ctx->ep_b, b);

	detect_opt(&(ctx->ep_opt_a), ctx->ep_a);
	detect_opt(&(ctx->ep_opt_b), ctx->ep_b);

	ep_norm(g, g);
	ep_copy(&(ctx->ep_g), g);
	bn_copy(&(ctx->ep_r), r);
	bn_copy(&(ctx->ep_h), h);

#if defined(EP_PRECO)
	ep_mul_pre(ep_curve_get_tab(), &(ctx->ep_g));
#endif
}

#endif

#if defined(EP_KBLTZ)

void ep_curve_set_kbltz(fp_t b, ep_t g, bn_t r, bn_t h, fp_t beta, bn_t l) {
	int bits = bn_bits(r);
	ctx_t *ctx = core_get();
	ctx->ep_is_kbltz = 1;

	fp_zero(ctx->ep_a);
	fp_copy(ctx->ep_b, b);

	detect_opt(&(ctx->ep_opt_a), ctx->ep_a);
	detect_opt(&(ctx->ep_opt_b), ctx->ep_b);

	ep_norm(g, g);
	ep_copy(&(ctx->ep_g), g);
	bn_copy(&(ctx->ep_r), r);
	bn_copy(&(ctx->ep_h), h);

#if EP_MUL == LWNAF || EP_FIX == COMBS || EP_FIX == LWNAF || EP_SIM == INTER || !defined(STRIP)
	fp_copy(ctx->beta, beta);
	bn_gcd_ext_mid(&(ctx->v1[1]), &(ctx->v1[2]), &(ctx->v2[1]), &(ctx->v2[2]), l, r);
	/* l = v1[1] * v2[2] - v1[2] * v2[1], r = l / 2. */
	bn_mul(&(ctx->v1[0]), &(ctx->v1[1]), &(ctx->v2[2]));
	bn_mul(&(ctx->v2[0]), &(ctx->v1[2]), &(ctx->v2[1]));
	bn_sub(l, &(ctx->v1[0]), &(ctx->v2[0]));
	bn_hlv(r, l);
	/* v1[0] = round(v2[2] * 2^|n| / l). */
	bn_lsh(&(ctx->v1[0]), &(ctx->v2[2]), bits + 1);
	if (bn_sign(&(ctx->v1[0])) == BN_POS) {
		bn_add(&(ctx->v1[0]), &(ctx->v1[0]), r);
	} else {
		bn_sub(&(ctx->v1[0]), &(ctx->v1[0]), r);
	}
	bn_div(&(ctx->v1[0]), &(ctx->v1[0]), l);
	/* v2[0] = round(v1[2] * 2^|n| / l). */
	bn_lsh(&(ctx->v2[0]), &(ctx->v1[2]), bits + 1);
	if (bn_sign(&(ctx->v2[0])) == BN_POS) {
		bn_add(&(ctx->v2[0]), &(ctx->v2[0]), r);
	} else {
		bn_sub(&(ctx->v2[0]), &(ctx->v2[0]), r);
	}
	bn_div(&(ctx->v2[0]), &(ctx->v2[0]), l);
	bn_neg(&(ctx->v2[0]), &(ctx->v2[0]));
#endif

#if defined(EP_PRECO)
	ep_mul_pre(ep_curve_get_tab(), &(ctx->ep_g));
#endif
}

#endif
