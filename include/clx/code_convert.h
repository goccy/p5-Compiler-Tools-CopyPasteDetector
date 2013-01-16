/* ------------------------------------------------------------------------- */
/*
 *  code_convert.h
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
 *  Last-modified: Thu 28 Aug 2008 00:02:02 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_CODE_CONVERT_H
#define CLX_CODE_CONVERT_H

#include "config.h"
#include <climits>
#include <string>
#include <locale>
#include <stdexcept>
#include "exception.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  widen
	/* --------------------------------------------------------------------- */
	inline std::basic_string<wchar_t> widen(const std::basic_string<char>& src) {
		std::basic_string<wchar_t> dest;
		for (std::size_t i = 1; i < src.size(); i += 2) {
			int c0 = src.at(i) & 0xff;
			int c1 = src.at(i - 1) & 0xff;
			wchar_t wc = (c1 << CHAR_BIT) | c0;
			dest += wc;
		}
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	//  narrow
	/* --------------------------------------------------------------------- */
	inline std::basic_string<char> narrow(const std::basic_string<wchar_t>& src) {
		std::basic_string<char> dest;
		for (std::size_t i = 0; i < src.size(); ++i) {
			char c0 = static_cast<char>(src.at(i));
			char c1 = static_cast<char>(src.at(i) >> CHAR_BIT);
			dest += c1;
			dest += c0;
		}
		return dest;
	}
	
	/* --------------------------------------------------------------------- */
	//  code_convert
	/* --------------------------------------------------------------------- */
	template <class Type, class CharT>
	inline std::basic_string<Type> code_convert(const std::basic_string<CharT>& src,
		const std::locale& loc = std::locale()) {
		return src;
	}
	
	template <class Type, class Source>
	inline std::basic_string<Type> code_convert(const Source* src,
		const std::locale& loc = std::locale()) {
		std::basic_string<Source> tmp(src);
		return code_convert<Type>(tmp, loc);
	}
	
#ifdef CLX_USE_WCHAR
	template <class Type, class CharT>
	inline std::basic_string<Type> code_convert(const std::basic_string<CharT>& src,
		const std::codecvt<wchar_t, char, std::mbstate_t>& cvt) {
		return src;
	}
	
	template <class Type, class Source, class State>
	inline std::basic_string<Type> code_convert(const Source* src,
		const std::codecvt<wchar_t, char, std::mbstate_t>& cvt) {
		std::basic_string<Source> tmp(src);
		return code_convert<Type>(tmp, cvt);
	}
	
	/* --------------------------------------------------------------------- */
	//  specialized code_convert: char -> wchar_t
	/* --------------------------------------------------------------------- */
	template <>
	inline std::basic_string<wchar_t> code_convert<wchar_t>(const std::basic_string<char>& src,
		const std::codecvt<wchar_t, char, std::mbstate_t>& cvt) {
		typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt_type;
		std::basic_string<wchar_t> dest;
		std::mbstate_t state(0);
		
		const char* src_beg = src.c_str();
		const char* src_end = src_beg + src.size();
		const char* src_next = src_beg;
		
		wchar_t* p = new wchar_t[(src.size() + 1) * sizeof(char)];
		if (p == NULL) throw std::bad_alloc();
		
		wchar_t* dest_beg = p;
		wchar_t* dest_end = p + (src.size() + 1) * sizeof(char);
		wchar_t* dest_next = p;
		
		while (1) {
			cvt_type::result res = cvt.in(state, src_beg, src_end, src_next,
				dest_beg, dest_end, dest_next);
			dest.append(dest_beg, dest_next - dest_beg);
			if (res == cvt_type::ok) break;
			else if (res == cvt_type::error) {
				delete [] p;
				throw std::runtime_error("convert failed");
			}
		}
		
		delete [] p;
		return dest;
	}
	
	template <>
	inline std::basic_string<wchar_t> code_convert<wchar_t>(const std::basic_string<char>& src,
		const std::locale& loc) {
		typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt_type;
		return code_convert<wchar_t>(src, std::use_facet<cvt_type>(loc));
	}
	
	/* --------------------------------------------------------------------- */
	//  specialized code_convert: wchar_t -> char
	/* --------------------------------------------------------------------- */
	template <>
	inline std::basic_string<char> code_convert<char>(const std::basic_string<wchar_t>& src,
		const std::codecvt<wchar_t, char, std::mbstate_t>& cvt) {
		typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt_type;
		std::basic_string<char> dest;
		std::mbstate_t state(0);
		
		const wchar_t* src_beg = src.c_str();
		const wchar_t* src_end = src_beg + src.size();
		const wchar_t* src_next = src_beg;
		
		char* p = new char[(src.size() + 1) * sizeof(wchar_t)];
		if (p == NULL) throw std::bad_alloc();
		
		char* dest_beg = p;
		char* dest_end = p + (src.size() + 1) * sizeof(wchar_t);
		char* dest_next = p;
		
		while (1) {
			cvt_type::result res = cvt.out(state, src_beg, src_end, src_next,
				dest_beg, dest_end, dest_next);
			dest.append(dest_beg, dest_next - dest_beg);
			if (res == cvt_type::ok) break;
			else if (res == cvt_type::error) {
				delete [] p;
				throw std::runtime_error("convert failed");
			}
		}
		
		delete [] p;
		return dest;
	}
	
	template <>
	inline std::basic_string<char> code_convert<char>(const std::basic_string<wchar_t>& src,
		const std::locale& loc) {
		typedef std::codecvt<wchar_t, char, std::mbstate_t> cvt_type;
		return code_convert<char>(src, std::use_facet<cvt_type>(loc));
	}
#endif // CLX_USE_WCHAR
}

#endif // CLX_CODE_CONVERT_H
