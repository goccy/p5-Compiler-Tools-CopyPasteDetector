/* ------------------------------------------------------------------------- */
/*
 *  memory.h
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
 *  Last-modified: Sat 14 Mar 2009 23:09:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_MEMORY_H
#define CLX_MEMORY_H

#include "config.h"
#include <cstring>
#include "endian.h"
#include "reverse.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  memcpy
	/* --------------------------------------------------------------------- */
	inline unsigned char* memcpy(unsigned char* dest, const unsigned char* src, int bytes) {
		std::memcpy(dest, src, bytes);
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	//  memcpy
	/* --------------------------------------------------------------------- */
	template <class Type>
	inline unsigned char* memcpy(unsigned char* dest, const Type* src, int bytes) {
		if (endian::is_little()) {
			std::memcpy(dest, reinterpret_cast<const unsigned char*>(src), bytes);
			return dest;
		}
		
		unsigned char* pos = dest;
		for (size_t i = 0; i < bytes / sizeof(Type); ++i) {
			Type tmp = reverse_copy(src[i]);
			std::memcpy(pos, &tmp, sizeof(Type));
			pos += sizeof(Type);
		}
		return dest;
	}
	
	template <class Type>
	inline char* memcpy(const char* dest, const Type* src, int bytes) {
		return memcpy(reinterpret_cast<unsigned char*>(dest), src, bytes);
	}
	
	/* --------------------------------------------------------------------- */
	//  memcpy
	/* --------------------------------------------------------------------- */
	template <class Type>
	inline Type* memcpy(Type* dest, const unsigned char* src, int bytes) {
		if (endian::is_little()) {
			std::memcpy(dest, reinterpret_cast<const unsigned char*>(src), bytes);
			return dest;
		}
		
		const unsigned char* pos = src;
		for (size_t i = 0; i < bytes / sizeof(Type); ++i) {
			std::reverse_copy(pos, pos + sizeof(Type), reinterpret_cast<unsigned char*>(&dest[i]));
			pos += sizeof(Type);
		}
		return dest;
	}
	
	template <class Type>
	inline Type* memcpy(Type* dest, const char* src, int bytes) {
		return memcpy(dest, reinterpret_cast<const unsigned char*>(src), bytes);
	}
}

#endif // CLX_MEMORY_H
