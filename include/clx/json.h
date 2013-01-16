/* ------------------------------------------------------------------------- */
/*
 *  json.h
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
 *  Last-modified: Sat 19 Jun 2010 01:58:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_JSON_H
#define CLX_JSON_H

#include "config.h"
#include <cctype>
#include <string>
#include <vector>
#include "literal.h"
#include "json_separator.h"
#include "tokenmap.h"
#include "predicate.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  json_array
	/* --------------------------------------------------------------------- */
	template <
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class basic_json_array {
	public:
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		typedef basic_tokenmap<json_separator<CharT, Traits>, string_type, string_type> object_type;
		typedef size_t size_type;
		
		json_control_charset<CharT> ctrl;
		
		basic_json_array() : obj_(), str_() {}
		
		template <class InIter>
		explicit basic_json_array(InIter first, InIter last) :
			obj_(), str_() {
			this->assign(first, last);
		}
		
		explicit basic_json_array(const string_type& src) :
			obj_(), str_() {
			this->assign(src);
		}
		
		explicit basic_json_array(const char_type* src) :
			obj_(), str_() {
			this->assign(src);
		}
		
		virtual ~basic_json_array() {}
		
		void reset() {
			obj_.clear();
			str_.clear();
		}
		
		template <class InIter>
		basic_json_array& assign(InIter first, InIter last) {
			if (*first != ctrl.lbracket) return *this;
			++first;
			
			object_type obj_tmp;
			string_type str_tmp;
			while (first != last) {
				if (*first == ctrl.rbracket) break;
				else if (*first == ctrl.lbrace) {
					string_type tmp;
					if (!detail::next_member(first, last, tmp, ctrl.lbrace, ctrl.rbrace)) return *this;
					obj_tmp.assign(tmp.begin(), tmp.end());
				}
				else if (*first == ctrl.lbracket) {
					if (!detail::next_member(first, last, str_tmp, ctrl.lbracket, ctrl.rbracket)) return *this;
				}
				else if (*first == ctrl.quot) {
					if (!detail::next_string(first, last, str_tmp)) return *this;
				}
				else if (*first == ctrl.comma) {
					if (!obj_tmp.empty()) obj_.push_back(obj_tmp);
					if (!str_tmp.empty()) str_.push_back(str_tmp);
					obj_tmp.reset();
					str_tmp.clear();
				}
				else if (!is_space()(*first)) str_tmp += *first;
				if (first == last) break;
				++first;
			}
			if (!obj_tmp.empty()) obj_.push_back(obj_tmp);
			if (!str_tmp.empty()) str_.push_back(str_tmp);
			return *this;
		}
		
		basic_json_array& assign(const string_type& src) {
			if (src.empty()) return *this;
			return this->assign(src.begin(), src.end());
		}
		
		basic_json_array& assign(const char_type* src) {
			string_type tmp(src);
			return this->assign(src);
		}
		
		bool empty() const { return obj_.empty() && str_.empty(); }
		
		std::vector<object_type>& objects() { return obj_; }
		const std::vector<object_type>& objects() const { return obj_; }
		
		object_type& object(size_type index) { return obj_.at(index); }
		const object_type& object(size_type index) const { return obj_.at(index); }
		
		std::vector<string_type>& strings() { return str_; }
		const std::vector<string_type>& strings() const { return str_; }
		
		string_type& string(size_type index) { return str_.at(index); }
		const string_type& string(size_type index) const { return str_.at(index); }
		
	private:
		std::vector<object_type> obj_;
		std::vector<string_type> str_;
	};
	
	typedef basic_tokenmap<json_separator<char>, std::string, std::string> json;
	typedef basic_json_array<char> json_array;
#ifdef CLX_USE_WCHAR
	typedef basic_tokenmap<json_separator<wchar_t>, std::wstring, std::wstring> wjson;
	typedef basic_json_array<wchar_t> wjson_array;
#endif // CLX_USE_WCHAR
}

#endif // CLX_JSON_H
