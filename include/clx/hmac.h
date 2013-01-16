/* ------------------------------------------------------------------------- */
/*
 *  hmac.h
 *
 *  Copyright (c) 2004 - 2008, clown. All rights reserved.
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
 *  Last-modified: Wed 29 Oct 2008 15:29:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_HMAC_H
#define CLX_HMAC_H

#include "config.h"
#include <cstring>
#include <string>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  hmac
	/* --------------------------------------------------------------------- */
	template <class Engine>
	inline Engine hmac(const char* key, size_t keylen, const char* data, size_t datalen) {
		unsigned char k_ipad[65];
		unsigned char k_opad[65];
		
		std::memset(k_ipad, 0, sizeof(k_ipad));
		std::memset(k_opad, 0, sizeof(k_opad));
		
		if (keylen > 64) {
			Engine ctx(key, keylen);
			std::memcpy(k_ipad, ctx.code(), ctx.size());
			std::memcpy(k_opad, ctx.code(), ctx.size());
		}
		else {
			std::memcpy(k_ipad, key, keylen);
			std::memcpy(k_opad, key, keylen);
		}
		
		for (int i = 0; i < 64; i++) {
			k_ipad[i] ^= 0x36;
			k_opad[i] ^= 0x5c;
		}
		
		Engine inner;
		inner.update(reinterpret_cast<const char*>(k_ipad), 64);
		inner.update(data, datalen);
		inner.finish();
		
		Engine outer;
		outer.update(reinterpret_cast<const char*>(k_opad), 64);
		outer.update(reinterpret_cast<const char*>(inner.code()), inner.size());
		outer.finish();
		
		return outer;
	}
	
	/* --------------------------------------------------------------------- */
	//  hmac
	/* --------------------------------------------------------------------- */
	template <class Engine>
	inline Engine hmac(const std::basic_string<char>& key, const std::basic_string<char>& data) {
		return hmac<Engine>(key.c_str(), key.size(), data.c_str(), data.size());
	}
}

#endif // CLX_HMAC_H
