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
 * Implementation of point negation on elliptic prime curves over quadratic
 * extensions.
 *
 * @version $Id: relic_pp_ep2.c 463 2010-07-13 21:12:13Z conradoplg $
 * @ingroup pp
 */

#include "relic_core.h"
#include "relic_md.h"
#include "relic_pp.h"
#include "relic_error.h"
#include "relic_conf.h"
#include "relic_fp_low.h"

/*============================================================================*/
	/* Public definitions                                                         */
/*============================================================================*/

#if EP_ADD == BASIC || !defined(STRIP)

void ep2_neg_basic(ep2_t r, ep2_t p) {
	if (ep2_is_infty(p)) {
		ep2_set_infty(r);
		return;
	}

	if (r != p) {
		fp2_copy(r->x, p->x);
		fp2_copy(r->z, p->z);
	}

	fp2_neg(r->y, p->y);

	r->norm = 1;
}

#endif

#if EP_ADD == PROJC || !defined(STRIP)

void ep2_neg_projc(ep2_t r, ep2_t p) {
	if (ep2_is_infty(p)) {
		ep2_set_infty(r);
		return;
	}

	if (r != p) {
		fp2_copy(r->x, p->x);
		fp2_copy(r->z, p->z);
	}

	fp2_neg(r->y, p->y);

	r->norm = p->norm;
}

#endif