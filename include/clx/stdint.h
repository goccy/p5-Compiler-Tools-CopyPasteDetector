/* ------------------------------------------------------------------------- */
/*
 *  stdint.h
 *
 *  Copyright (c) 2004 - 2010, clown. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - No names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Last-modified: Fri 19 Mar 2010 21:31:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_STDINT_H
#define CLX_STDINT_H

#include "config.h"

#if defined(HAVE_TR1_CSTDINT) || defined(HAS_TR1_CSTDINT)
#include <tr1/cstdint>

namespace clx {
	namespace stdint_ = std::tr1;
	
	using ::stdint_::int8_t;
	using ::stdint_::uint8_t;
	using ::stdint_::int_least8_t;
	using ::stdint_::uint_least8_t;
	using ::stdint_::int_fast8_t;
	using ::stdint_::uint_fast8_t;

	using ::stdint_::int16_t;
	using ::stdint_::uint16_t;
	using ::stdint_::int_least16_t;
	using ::stdint_::uint_least16_t;
	using ::stdint_::int_fast16_t;
	using ::stdint_::uint_fast16_t;

	using ::stdint_::int32_t;
	using ::stdint_::uint32_t;
	using ::stdint_::int_least32_t;
	using ::stdint_::uint_least32_t;
	using ::stdint_::int_fast32_t;
	using ::stdint_::uint_fast32_t;
	
#ifdef CLX_HAS_LONG_LONG
	using ::stdint_::int64_t;
	using ::stdint_::uint64_t;
	using ::stdint_::int_least64_t;
	using ::stdint_::uint_least64_t;
	using ::stdint_::int_fast64_t;
	using ::stdint_::uint_fast64_t;
#endif
	
	using ::stdint_::intmax_t;
	using ::stdint_::uintmax_t;
}

#else
#include "stdint_alternative.h"
#endif

#endif // CLX_STDINT_H
