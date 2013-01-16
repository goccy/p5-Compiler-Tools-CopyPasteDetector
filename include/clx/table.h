/* ------------------------------------------------------------------------- */
/*
 *  table.h
 *
 *  Copyright (c) 2004 - 2009, clown. All rights reserved.
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
 *  Last-modified: Tue 03 Nov 2009 21:53:03 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_TABLE_H
#define CLX_TABLE_H

#include "config.h"
#include <string>
#include <vector>
#include <istream>
#include <fstream>
#include <stdexcept>
#include "config.h"
#include "container_generator.h"
#include "container_accessor.h"

namespace clx {
	namespace detail {
		template <class Container, class Source>
		class parser_base {
		public:
			parser_base() {}
			virtual ~parser_base() {}
			virtual const Container& run(const Source& src) = 0;
		};
	};
	
	/* --------------------------------------------------------------------- */
	//  table
	/* --------------------------------------------------------------------- */
	template <
		class Type,
		class Source = std::string,
		class Container = std::vector<std::vector<Type> >
	>
	class table :
		private container_generator<Container>,
		public random_accessor<Container> {
	private:
		typedef container_generator<Container> generator;
		typedef random_accessor<Container> accessor;
	public:
		typedef Type value_type;
		typedef Source string_type;
		typedef Container container;
		typedef typename container::value_type subcontainer;
		
		// constructor and destructor
		table() : generator(), accessor(this->member), p_() {}
		
		table(const table& cp) :
			generator(cp.member), accessor(this->member), p_(cp.p_) {}
		
		template <class Functor>
		explicit table(Functor f) :
			generator(), accessor(this->member), p_(new parser<Functor>(f)) {}
		
		template <class Functor>
		table(const string_type& path, Functor f) :
			generator(), accessor(this->member), p_(new parser<Functor>(f)) {
			this->read(path);
		}
		
		template <class Ch, class Tr, class Functor>
		table(std::basic_istream<Ch, Tr>& sin, Functor f) :
			generator(), accessor(this->member), p_(new parser<Functor>(f)) {
			this->read(sin);
		}
		
		virtual ~table() throw() { if (p_) delete p_; }
		
		table& operator=(const table& cp) {
			this->member = cp.member;
			p_ = cp.p_;
			return *this;
		}
		
		// operator
		template <class Ch, class Tr>
		bool readline(std::basic_istream<Ch, Tr>& sin) {
			string_type tmp;
			if (!std::getline(sin, tmp)) return false;
			if (p_) this->member.insert(this->member.end(), p_->run(tmp));
			return true;
		}
		
		template <class Ch, class Tr>
		table& read(std::basic_istream<Ch, Tr>& sin) {
			while (this->readline(sin));
			return *this;
		}
		
		table& read(const string_type& path) {
			std::basic_ifstream<typename string_type::value_type,
				typename string_type::traits_type> fin(path);
			if (!fin.is_open()) throw std::runtime_error("No such file or directory");
			this->read(fin);
			fin.close();
			return *this;
		}
		
		void reset() { this->member.clear(); }
		
	private:
		typedef detail::parser_base<typename Container::value_type, string_type>* parser_ptr;
		
		parser_ptr p_;
		
		template <class F>
		class parser : public detail::parser_base<typename Container::value_type, string_type> {
		public:
			typedef typename Container::value_type container;
			parser(F f) : f_(f) {}
			virtual ~parser() {}
			virtual const container& run(const string_type& src) { return f_(src); }
		private:
			F f_;
		};
	};
}

#endif // CLX_TABLE_H
