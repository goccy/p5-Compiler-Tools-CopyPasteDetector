/* ------------------------------------------------------------------------- */
/*
 *  wsse.h
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
 *  Last-modified: Thu 09 Oct 2008 07:50:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_WSSE_H
#define CLX_WSSE_H

#include "config.h"
#include <ctime>
#include <string>
#include <sstream>
#include "time.h"
#include "base64.h"
#include "sha1.h"
#include "random.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  wsse
	/* --------------------------------------------------------------------- */
	inline std::string wsse(const std::string& id, const std::string& password) {
		std::string nonce;
		mt19937 engine(static_cast<unsigned long>(std::time(NULL)));
		random<char> rand(engine);
		for (int i = 0; i < 20; ++i) nonce.push_back(rand() >> 1);
		std::string nonce_base64 = clx::base64::encode(nonce);
		
		clx::date_time_utc now;
		std::string created = now.to_string<char>("%Y-%m-%dT%H:%M:%SZ");
		clx::sha1 sha(nonce + created + password);
		std::string digest = clx::base64::encode((char*)sha.code());
		
		std::stringstream ss;
		ss << "UsernameToken Username=\"" << id << "\", PasswordDigest=\""
			<< digest << "\", Nonce=\"" << nonce_base64 << "\", Created=\"" << created << '"';
			
		return ss.str();
	}
	
	/* --------------------------------------------------------------------- */
	//  wsse
	/* --------------------------------------------------------------------- */
	inline std::string wsse(const char* id, const char* password) {
		std::string tmp_id(id);
		std::string tmp_pass(password);
		return wsse(tmp_id, tmp_pass);
	}
}

#endif // CLX_WSSE_H
