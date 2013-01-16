/* ------------------------------------------------------------------------- */
/*
 *  font.h
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
 *  Last-modified: Thu 28 Jan 2010 14:18:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_FONT_H
#define CLX_FONT_H

#include "config.h"
#include <string>
#include <istream>
#include <fstream>
#include "utility.h"
#include "mpl/combine.h"

namespace clx {
	namespace detail {
		/* ----------------------------------------------------------------- */
		/*
		 *  ttf_checksum
		 *
		 *  The file checksum of TrueType font should be "0xb1b0afba".
		 */
		/* ----------------------------------------------------------------- */
		inline std::size_t ttf_checksum(std::basic_istream<char>& in) {
			std::streamoff cur = in.tellg();
			in.seekg(0);
			std::size_t sum = 0, chunk = 0;
			while (clx::get(in, chunk, clx::endian::big)) sum += chunk;
			in.clear();
			in.seekg(cur);
			return sum;
		}
	} // namespace detail
	
	/* --------------------------------------------------------------------- */
	/*
	 *  is_ttf
	 *
	 *  Check if the file format is TrueType font (*.ttf).
	 */
	/* --------------------------------------------------------------------- */
	inline bool is_ttf(std::basic_istream<char>& in) {
		std::size_t sig = 0;
		if (!clx::get(in, sig, clx::endian::big)) return false;
		if (sig == clx::mpl::combine4<'t', 't', 'c', 'f'>::value) return false;
		
		unsigned short count = 0;
		if (!clx::get(in, count, clx::endian::big)) return false;
		
		if (sig == clx::mpl::combine4<'S', 'p', 'l', 'i'>::value ||
			sig == clx::mpl::combine4<'%', '!', 'P', 'S'>::value ||
			sig == clx::mpl::combine4<'t', 'y', 'p', '1'>::value) {
			return false;
		}
		else if (sig >= 0x80000000 && count == 0) return false;
		else if ((sig >> 24) == 1 && ((sig >> 16) & 0xff) == 0 && ((sig >> 8) & 0xff) == 4) return false; // CFF?
		
		if (detail::ttf_checksum(in) != 0xb1b0afba) return false; // file checksum
		
		return true; // "OTTO", "true", or others.
	}
	
	/* --------------------------------------------------------------------- */
	//  is_ttf
	/* --------------------------------------------------------------------- */
	inline bool is_ttf(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_ttf(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_ttf
	/* --------------------------------------------------------------------- */
	inline bool is_ttf(const std::basic_string<char>& path) {
		return clx::is_ttf(path.c_str());
	}
	
	/* --------------------------------------------------------------------- */
	/*
	 *  is_ttc
	 *
	 *  Check if the file format is TrueType collection (*.ttc).
	 */
	/* --------------------------------------------------------------------- */
	inline bool is_ttc(std::basic_istream<char>& in) {
		std::size_t sig = 0;
		if (!clx::get(in, sig, clx::endian::big)) return false;
		if (sig == clx::mpl::combine4<'t', 't', 'c', 'f'>::value) return true;
		return false;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_ttc
	/* --------------------------------------------------------------------- */
	inline bool is_ttc(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_ttc(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_ttc
	/* --------------------------------------------------------------------- */
	inline bool is_ttc(const std::basic_string<char>& path) {
		return clx::is_ttc(path.c_str());
	}
} // namespace clx

#endif // CLX_FONT_H
