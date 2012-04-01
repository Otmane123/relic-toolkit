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

/**
 * @file
 *
 * Implementation of hashing to a prime elliptic curve over a quadratic
 * extension.
 *
 * @version $Id$
 * @ingroup pp
 */

#include "relic_core.h"
#include "relic_md.h"
#include "relic_pp.h"
#include "relic_error.h"
#include "relic_conf.h"
#include "relic_fp_low.h"

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

/**
 * Multiplies a point by the curve cofactor.
 *
 * @param[out] r			- the result.
 * @param[in] p				- the point to multiply.
 */
void ep2_mul_cof(ep2_t r, ep2_t p) {
	bn_t x;
	ep2_t t0, t1, t2;

	ep2_null(t0);
	ep2_null(t1);
	ep2_null(t2);
	bn_null(x);

	TRY {
		ep2_new(t0);
		ep2_new(t1);
		ep2_new(t2);
		bn_new(x);

		fp_param_get_var(x);

		/* Compute t0 = xP. */
		ep2_mul(t0, p, x);
		if (bn_sign(x) == BN_NEG) {
			ep2_neg(t0, t0);
		}

		/* Compute t1 = p(3xP). */
		ep2_dbl(t1, t0);
		ep2_add(t1, t1, t0);
		ep2_norm(t1, t1);
		ep2_frb(t1, t1, 1);

		/* Compute t2 = p^3(P) + t0 + t1 + p^2(xP). */
		ep2_frb(t2, p, 2);
		ep2_frb(t2, t2, 1);
		ep2_add(t2, t2, t0);
		ep2_add(t2, t2, t1);
		ep2_frb(t1, t0, 2);
		ep2_add(t2, t2, t1);

		ep2_norm(r, t2);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		ep2_free(t0);
		ep2_free(t1);
		ep2_free(t2);
		bn_free(x);
	}
}

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void ep2_map(ep2_t p, unsigned char *msg, int len) {
	bn_t x;
	fp2_t t0;
	unsigned char digest[MD_LEN];

	bn_null(x);
	fp2_null(t0);

	TRY {
		bn_new(x);
		fp2_new(t0);

		md_map(digest, msg, len);
		memcpy(x->dp, digest, MIN(FP_BYTES, MD_LEN));

		fp_prime_conv(p->x[0], x);
		fp_zero(p->x[1]);
		fp_set_dig(p->z[0], 1);
		fp_zero(p->z[1]);

		while (1) {
			ep2_rhs(t0, p);

			if (fp2_srt(p->y, t0)) {
				p->norm = 1;
				break;
			}

			fp_add_dig(p->x[0], p->x[0], 1);
		}

		ep2_mul_cof(p, p);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		bn_free(x);
		fp2_free(t0);
	}
}
