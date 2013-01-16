/* ------------------------------------------------------------------------- */
/*
 *  ini.h
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
 *  Last-modified: Fri 22 Jan 2010 17:29:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_INI_H
#define CLX_INI_H

#include "config.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stdexcept>
#include "exception.h"
#include "literal.h"
#include "lexical_cast.h"
#include "salgorithm.h"
#include "container_wrapper.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  ini_container
	/* --------------------------------------------------------------------- */
	template <
		class Type,
		class CharT,
		class Traits = std::char_traits<CharT>
	>
	class ini_container : public map_wrapper<std::basic_string<CharT, Traits>, Type> {
	public:
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		
		typedef map_wrapper<std::basic_string<CharT, Traits>, Type> super;
		typedef typename super::container container;
		typedef typename container::size_type size_type;
		typedef typename container::key_type key_type;
		typedef typename container::mapped_type mapped_type;
		typedef typename container::value_type value_type;
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
		
		ini_container() : super() {}
		
		virtual ~ini_container() throw() {}
		
		bool insert(const key_type& s) {
			std::vector<string_type> v;
			split_if(s, v, is_any_of(LITERAL("=")));
			if (v.empty()) return false;
			
			string_type key = v.at(0);
			string_type val;
			if (v.size() >= 2) val = v.at(1);
			
			value_type elem(key, lexical_cast<mapped_type>(val));
			std::pair<iterator, bool> ret = this->member.insert(elem);
			return ret.second;
		}
		
		bool exist(const key_type& x) const {
			return this->member.find(x) != this->member.end();
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_ini
	/* --------------------------------------------------------------------- */
	template <
		class Type,
		class CharT = char,
		class Traits = std::char_traits<CharT>
	>
	class basic_ini : public map_wrapper<std::basic_string<CharT, Traits>,
		ini_container<Type, CharT, Traits> > {
	private:
		typedef map_wrapper<std::basic_string<CharT, Traits>,
			ini_container<Type, CharT, Traits> > super;
	public:
		typedef CharT char_type;
		typedef std::basic_string<CharT, Traits> string_type;
		
		typedef string_type key_type;
		typedef Type value_type;
		
		typedef ini_container<Type, CharT, Traits> subcontainer;
		typedef typename subcontainer::iterator sub_iterator;
		typedef typename subcontainer::const_iterator const_sub_iterator;
		
		basic_ini() : super() {}
		
		template <class Ch, class Tr>
		explicit basic_ini(std::basic_istream<Ch, Tr>& sin) : super() {
			this->read(sin);
		}
		
		explicit basic_ini(const string_type& path) : super() {
			this->read(path);
		}
		
		explicit basic_ini(const char_type* path) : super() {
			this->read(path);
		}
		
		virtual ~basic_ini() throw() {}
		
		template <class Ch, class Tr>
		basic_ini& read(std::basic_istream<Ch, Tr>& sin) {
			const char_type semi = LITERAL(';');
			const char_type lb = LITERAL('[');
			const char_type rb = LITERAL(']');
			
			string_type cur;
			size_t n = 0;
			
			string_type tmp;
			while (std::getline(sin, tmp)) {
				chomp(tmp);
				++n;
				
				if (tmp.empty() || tmp.at(0) == semi) continue;
				else if (tmp.at(0) == lb) { // section
					size_t last = tmp.find(rb);
					if (last == string_type::npos) throw syntax_error(n, "expected ']' token");
					cur = tmp.substr(1, last - 1);
					this->insert(cur);
				}
				else { // parser and insert the string "key=value"
					if (this->member.find(cur) == this->member.end()) this->insert(cur);
					if (!this->member[cur].insert(tmp)) throw syntax_error(n, "unknown error");
				}
			}
			
			return *this;
		}
		
		basic_ini& read(const char_type* path) {
			std::basic_ifstream<CharT, Traits> fs(path);
			if (fs.fail()) {
				std::basic_stringstream<CharT, Traits> msg;
				msg << path << ": no such file or directory";
				throw std::runtime_error(msg.str());
			}
			return this->read(fs);
		}
		
		basic_ini& read(const string_type& path) {
			if (path.empty()) return *this;
			return this->read(path.c_str());
		}
		
		template <class Ch, class Tr>
		basic_ini& write(std::basic_ostream<Ch, Tr>& out) {
			typename super::iterator pos = this->member.find(string_type());
			if (pos != this->member.end()) {
				for (typename subcontainer::iterator it = pos->second.begin();
					it != pos->second.end(); ++it) {
					out << it->first << LITERAL("=") << it->second << std::endl;
				}
			}
			
			for (pos = this->member.begin(); pos != this->member.end(); ++pos) {
				if (pos->first == string_type()) continue;
				out << LITERAL("[") << pos->first << LITERAL("]") << std::endl;
				for (typename subcontainer::iterator it = pos->second.begin();
					it != pos->second.end(); ++it) {
					out << it->first << LITERAL("=") << it->second << std::endl;
				}
			}
			return *this;
		}
		
		basic_ini& write(const char_type* path) {
			std::basic_ofstream<CharT, Traits> ofs(path);
			if (ofs.fail()) {
				std::basic_stringstream<CharT, Traits> msg;
				msg << path << ": cannot open the file";
				throw std::runtime_error(msg.str());
			}
			return this->write(ofs);
		}
		
		basic_ini& write(const string_type& path) {
			if (path.empty()) return *this;
			return this->write(path.c_str());
		}
		
		// make new section
		bool insert(const key_type& section) {
			std::pair<typename super::iterator, bool> ret;
			std::pair<key_type, subcontainer> elem(section, subcontainer());
			ret = this->member.insert(elem);
			return ret.second;
		}
		
		bool exist(const key_type& section) const {
			return this->member.find(section) != this->member().end();
		}
		
		bool exist(const key_type& section, const key_type& key) const {
			typename super::const_iterator p = this->member.find(section);
			if (p == this->member.end()) return false;
			return p->second.exist(key);
		}
	};
	
	typedef basic_ini<std::basic_string<char> > ini;
#ifdef CLX_USE_WCHAR
	typedef basic_ini<std::basic_string<wchar_t> > wini;
#endif // CLX_USE_WCHAR
}

#endif // CLX_INI_H
