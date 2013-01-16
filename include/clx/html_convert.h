/* ------------------------------------------------------------------------- */
/*
 *  html_convert.h
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
 *  Last-modified: Mon 01 Mar 2010 19:43:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_HTML_CONVERT_H
#define CLX_HTML_CONVERT_H

#include "config.h"
#include <stdexcept>
#include <algorithm>
#include <map>
#include "literal.h"
#include "lexical_cast.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_html_encoder
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_html_encoder {
	public:
		typedef std::size_t size_type;
		typedef CharT char_type;
		typedef Traits traits;
		typedef std::basic_string<CharT, Traits> string_type;
		
		basic_html_encoder() :
			v_() {
			v_[LITERAL('&')] = LITERAL("&amp;");
			v_[LITERAL('"')] = LITERAL("&quot;");
			v_[LITERAL('<')] = LITERAL("&lt;");
			v_[LITERAL('>')] = LITERAL("&gt;");
			v_[LITERAL('\'')] = LITERAL("&#39;");
		}
		
		explicit basic_html_encoder(const std::map<char_type, string_type>& cp) :
			v_(cp) {}
		
		~basic_html_encoder() throw() {}
		
		template <class OutIter>
		OutIter operator()(char_type c, OutIter out) {
			typename std::map<char_type, string_type>::const_iterator pos = v_.find(c);
			if (pos != v_.end()) out = std::copy(pos->second.begin(), pos->second.end(), out);
			else *out++ = c;
			return out;
		}
		
		template <class OutIter>
		OutIter reset(OutIter out) {
			return out;
		}
		
		template <class OutIter>
		OutIter finish(OutIter out) {
			return out;
		}
		
	private:
		std::map<char_type, string_type> v_;
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_html_decoder
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_html_decoder {
	public:
		typedef std::size_t size_type;
		typedef CharT char_type;
		typedef Traits traits;
		typedef std::basic_string<CharT, Traits> string_type;
		
		basic_html_decoder() :
			escaped_(false), buffer_(), v_() {
			v_[LITERAL("amp")] = LITERAL('&');
			v_[LITERAL("quot")] = LITERAL('"');
			v_[LITERAL("lt")] = LITERAL('<');
			v_[LITERAL("gt")] = LITERAL('>');
		}
		
		~basic_html_decoder() throw() {}
		
		template <class OutIter>
		OutIter operator()(char_type c, OutIter out) {
			if (escaped_) {
				if (c == LITERAL(';')) {
					typename std::map<string_type, char_type>::const_iterator pos = v_.find(buffer_);
					if (pos != v_.end()) *out++ = pos->second;
					else {
						try {
							size_type code = lexical_cast<size_type>(buffer_, std::ios::showbase);
							*out++ = static_cast<char_type>(code);
						}
						catch (clx::bad_lexical_cast& e) {
							throw std::runtime_error("invalid escaped character");
						}
					}
					buffer_.clear();
					escaped_ = 0;
				}
				else if (c == LITERAL('#')) escaped_ = 2;
				else if (c == LITERAL('x') && escaped_ == 2) buffer_ += LITERAL("0x");
				else buffer_ += c;
			}
			else {
				if (c == LITERAL('&')) escaped_ = 1;
				else *out++ = c;
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
			if (!buffer_.empty()) throw std::runtime_error("invalid escaped character");
			return out;
		}
		
	private:
		size_type escaped_;
		string_type buffer_;
		std::map<string_type, char_type> v_;
	};
	
	typedef basic_html_encoder<char, std::char_traits<char> > html_encoder;
	typedef basic_html_decoder<char, std::char_traits<char> > html_decoder;
#ifdef CLX_USE_WCHAR
	typedef basic_html_encoder<wchar_t, std::char_traits<wchar_t> > whtml_encoder;
	typedef basic_html_decoder<wchar_t, std::char_traits<wchar_t> > whtml_decoder;
#endif
} // namespace clx

#endif // CLX_HTML_CONVERT_H
