/* ------------------------------------------------------------------------- */
/*
 *  base64_convert.h
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
 *  Last-modified: Sat 06 Feb 2010 05:15:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_BASE64_CONVERT_H
#define CLX_BASE64_CONVERT_H

#include "config.h"
#include <cassert>
#include <string>
#include <stdexcept>
#include "literal.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_base64_encoder
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_base64_encoder {
	public:
		typedef std::size_t size_type;
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		
		basic_base64_encoder() : buffer_() {}
		
		~basic_base64_encoder() throw() {}
		
		template <class OutIter>
		OutIter operator()(char_type c, OutIter out) {
			assert(buffer_.size() < 3);
			
			buffer_ += c;
			if (buffer_.size() == 3) {
				*out++ = table().at((buffer_.at(0) & 0xfc) >> 2);
				*out++ = table().at(((buffer_.at(0) & 0x03) << 4) | ((buffer_.at(1) & 0xf0) >> 4));
				*out++ = table().at(((buffer_.at(1) & 0x0f) << 2) | ((buffer_.at(2) & 0xc0) >> 6));
				*out++ = table().at((buffer_.at(2) & 0x3f));
				buffer_.clear();
			}
			
			return out;
		}
		
		template <class OutIter>
		OutIter reset(OutIter out) {
			buffer_.clear();
			return out;
		}
		
		template <class OutIter>
		OutIter finish(OutIter out) {
			assert(buffer_.size() < 3);
			
			switch (buffer_.size()) {
			case 1:
				buffer_ += static_cast<char_type>(0);
				buffer_ += static_cast<char_type>(0);
				*out++ = table().at((buffer_.at(0) & 0xfc) >> 2);
				*out++ = table().at(((buffer_.at(0) & 0x03) << 4) | ((buffer_.at(1) & 0xf0) >> 4));
				*out++ = table().at(64);
				*out++ = table().at(64);
				break;
			case 2:
				buffer_ += static_cast<char_type>(0);
				*out++ = table().at((buffer_.at(0) & 0xfc) >> 2);
				*out++ = table().at(((buffer_.at(0) & 0x03) << 4) | ((buffer_.at(1) & 0xf0) >> 4));
				*out++ = table().at(((buffer_.at(1) & 0x0f) << 2) | ((buffer_.at(2) & 0xc0) >> 6));
				*out++ = table().at(64);
				break;
			default:
				break;
			}
			
			buffer_.clear();
			return out;
		}
		
	private:
		string_type buffer_;
		
		static const string_type& table() {
			static const string_type s(LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="));
			return s;
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_base64_decoder
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_base64_decoder {
	public:
		typedef std::size_t size_type;
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		
		basic_base64_decoder() : buffer_() {}
		
		~basic_base64_decoder() throw() {}
		
		template <class OutIter>
		OutIter operator()(char_type c, OutIter out) {
			assert(buffer_.size() < 4);
			
			if (c != table().at(64)) {
				size_type pos = table().find(c);
				if (pos == string_type::npos) throw std::runtime_error("wrong base64 encoded data");
				buffer_ += static_cast<char_type>(pos);
			}
			
			if (buffer_.size() == 4) {
				*out++ = ((buffer_.at(0) << 2) & 0xfc) | ((buffer_.at(1) >> 4) & 0x03);
				*out++ = ((buffer_.at(1) << 4) & 0xf0) | ((buffer_.at(2) >> 2) & 0x0f);
				*out++ = ((buffer_.at(2) << 6) & 0xc0) | (buffer_.at(3) & 0x3f);
				buffer_.clear();
			}
			
			return out;
		}
		
		template <class OutIter>
		OutIter reset(OutIter out) {
			buffer_.clear();
			return out;
		}
		
		template <class OutIter>
		OutIter finish(OutIter out) {
			if (!buffer_.empty()) {
				assert(buffer_.size() >= 2 && buffer_.size() <= 4);
				
				string_type tmp(buffer_);
				buffer_.clear();
				
				*out++ = ((tmp.at(0) << 2) & 0xfc) | ((tmp.at(1) >> 4) & 0x03);
				if (tmp.size() == 2) return out;
				*out++ = ((tmp.at(1) << 4) & 0xf0) | ((tmp.at(2) >> 2) & 0x0f);
				if (tmp.size() == 3) return out;
				*out++ = ((tmp.at(2) << 6) & 0xc0) | (tmp.at(3) & 0x3f);
				
			}
			
			return out;
		}
		
	private:
		string_type buffer_;
		
		static const string_type& table() {
			static const string_type s(LITERAL("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="));
			return s;
		}
	};
	
	typedef basic_base64_encoder<char, std::char_traits<char> > base64_encoder;
	typedef basic_base64_decoder<char, std::char_traits<char> > base64_decoder;
} // namespace clx

#endif // CLX_BASE64_CONVERT_H
