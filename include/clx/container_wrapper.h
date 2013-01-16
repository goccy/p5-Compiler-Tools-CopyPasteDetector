/* ------------------------------------------------------------------------- */
/*
 *  container_wrapper.h
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
 *  Last-modified: Wed 04 Nov 2009 13:38:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_CONTAINER_WRAPPER_H
#define CLX_CONTAINER_WRAPPER_H

#include "config.h"
#include "container_generator.h"
#include "container_accessor.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	/*
	 *  vector_wrapper
	 *
	 *  The container_wrapper classes (vector_wrapper, deque_wrapper,
	 *  list_wrapper, set_wrapper, multiset_wrapper, map_wrapper,
	 *  multimap_wrapper) are used when you inherit STL container classes.
	 */
	/* --------------------------------------------------------------------- */
	template <
		class Type,
		class Alloc = std::allocator<Type>
	>
	class vector_wrapper :
		protected container_generator<std::vector<Type, Alloc> >,
		public vector_accessor<std::vector<Type, Alloc> > {
	private:
		typedef container_generator<std::vector<Type, Alloc> > generator;
		typedef vector_accessor<std::vector<Type, Alloc> > accessor;
	
	public:
		typedef typename std::vector<Type, Alloc> container;
		typedef typename container::size_type size_type;
		
		vector_wrapper(const vector_wrapper& cp) :
			generator(cp.member), accessor(this->member) {}
		
		explicit vector_wrapper(const container& cp) :
			generator(cp), accessor(this->member) {}
		
		explicit vector_wrapper(const Alloc& alloc = Alloc()) :
			generator(alloc), accessor(this->member) {}
		
		explicit vector_wrapper(size_type n, const Type& value = Type(), const Alloc& alloc = Alloc()) :
			generator(n, value, alloc), accessor(this->member) {}
		
		template <class InIter>
		vector_wrapper(InIter first, InIter last, const Alloc& alloc = Alloc()) :
			generator(first, last, alloc), accessor(this->member) {}
		
		virtual ~vector_wrapper() throw() {}
		
		vector_wrapper& operator=(const vector_wrapper& cp) {
			this->member = cp.member;
			return *this;
		}
		
		vector_wrapper& operator=(const container& cp) {
			this->member = cp;
			return *this;
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  deque_wrapper
	/* --------------------------------------------------------------------- */
	template <
		class Type,
		class Alloc = std::allocator<Type>
	>
	class deque_wrapper :
		protected container_generator<std::deque<Type, Alloc> >,
		public deque_accessor<std::deque<Type, Alloc> > {
	private:
		typedef container_generator<std::deque<Type, Alloc> > generator;
		typedef deque_accessor<std::deque<Type, Alloc> > accessor;
	
	public:
		typedef typename std::deque<Type, Alloc> container;
		typedef typename container::size_type size_type;
		
		deque_wrapper(const deque_wrapper& cp) :
			generator(cp.member), accessor(this->member) {}
		
		explicit deque_wrapper(const container& cp) :
			generator(cp), accessor(this->member) {}
		
		explicit deque_wrapper(const Alloc& alloc = Alloc()) :
			generator(alloc), accessor(this->member) {}
		
		explicit deque_wrapper(size_type n, const Type& value = Type(), const Alloc& alloc = Alloc()) :
			generator(n, value, alloc), accessor(this->member) {}
		
		virtual ~deque_wrapper() throw() {}
		
		deque_wrapper& operator=(const deque_wrapper& cp) {
			this->member = cp.member;
			return *this;
		}
		
		deque_wrapper& operator=(const container& cp) {
			this->member = cp;
			return *this;
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  list_wrapper
	/* --------------------------------------------------------------------- */
	template <
		class Type,
		class Alloc = std::allocator<Type>
	>
	class list_wrapper :
		protected container_generator<std::list<Type, Alloc> >,
		public list_accessor<std::list<Type, Alloc> > {
	private:
		typedef container_generator<std::list<Type, Alloc> > generator;
		typedef list_accessor<std::list<Type, Alloc> > accessor;
	
	public:
		typedef typename std::list<Type, Alloc> container;
		typedef typename container::size_type size_type;
		
		list_wrapper(const list_wrapper& cp) :
			generator(cp.member), accessor(this->member) {}
		
		explicit list_wrapper(const container& cp) :
			generator(cp), accessor(this->member) {}
		
		explicit list_wrapper(const Alloc& alloc = Alloc()) :
			generator(alloc), accessor(this->member) {}
		
		explicit list_wrapper(size_type n, const Type& value = Type(), const Alloc& alloc = Alloc()) :
			generator(n, value, alloc), accessor(this->member) {}
		
		virtual ~list_wrapper() throw() {}
		
		list_wrapper& operator=(const list_wrapper& cp) {
			this->member = cp.member;
			return *this;
		}
		
		list_wrapper& operator=(const container& cp) {
			this->member = cp;
			return *this;
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  set_wrapper
	/* --------------------------------------------------------------------- */
	template <
		class Key,
		class Compare = std::less<Key>,
		class Alloc = std::allocator<Key>
	>
	class set_wrapper :
		protected comparable_container_generator<std::set<Key, Compare, Alloc> >,
		public set_accessor<std::set<Key, Compare, Alloc> > {
	private:
		typedef comparable_container_generator<std::set<Key, Compare, Alloc> > generator;
		typedef set_accessor<std::set<Key, Compare, Alloc> > accessor;
		
	public:
		typedef std::set<Key, Compare, Alloc> container;
		
		set_wrapper(const set_wrapper& cp) :
			generator(cp.member), accessor(this->member) {}
		
		explicit set_wrapper(const container& cp) :
			generator(cp), accessor(this->member) {}
		
		explicit set_wrapper(const Compare& comp = Compare(), const Alloc& alloc = Alloc()) :
			generator(comp, alloc), accessor(this->member) {}
		
		template <class InIter>
		explicit set_wrapper(InIter first, InIter last,
			const Compare& comp = Compare(), const Alloc& alloc = Alloc()) :
			generator(first, last, comp, alloc), accessor(this->member) {}
		
		virtual ~set_wrapper() throw() {}
		
		set_wrapper& operator=(const set_wrapper& cp) {
			this->member = cp.member;
			return *this;
		}
		
		set_wrapper& operator=(const container& cp) {
			this->member = cp;
			return *this;
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  multiset_wrapper
	/* --------------------------------------------------------------------- */
	template <
		class Key,
		class Compare = std::less<Key>,
		class Alloc = std::allocator<Key>
	>
	class multiset_wrapper :
		protected comparable_container_generator<std::multiset<Key, Compare, Alloc> >,
		public set_accessor<std::multiset<Key, Compare, Alloc> > {
	private:
		typedef comparable_container_generator<std::multiset<Key, Compare, Alloc> > generator;
		typedef set_accessor<std::multiset<Key, Compare, Alloc> > accessor;
		
	public:
		typedef std::multiset<Key, Compare, Alloc> container;
		
		multiset_wrapper(const multiset_wrapper& cp) :
			generator(cp.member), accessor(this->member) {}
		
		explicit multiset_wrapper(const container& cp) :
			generator(cp), accessor(this->member) {}
		
		explicit multiset_wrapper(const Compare& comp = Compare(), const Alloc& alloc = Alloc()) :
			generator(comp, alloc), accessor(this->member) {}
		
		template <class InIter>
		explicit multiset_wrapper(InIter first, InIter last,
			const Compare& comp = Compare(), const Alloc& alloc = Alloc()) :
			generator(first, last, comp, alloc), accessor(this->member) {}
		
		virtual ~multiset_wrapper() throw() {}
		
		multiset_wrapper& operator=(const multiset_wrapper& cp) {
			this->member = cp.member;
			return *this;
		}
		
		multiset_wrapper& operator=(const container& cp) {
			this->member = cp;
			return *this;
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  map_wrapper
	/* --------------------------------------------------------------------- */
	template <
		class Key,
		class Type,
		class Compare = std::less<Key>,
		class Alloc = std::allocator<Key>
	>
	class map_wrapper :
		protected comparable_container_generator<std::map<Key, Type, Compare, Alloc> >,
		public map_accessor<std::map<Key, Type, Compare, Alloc> > {
	private:
		typedef comparable_container_generator<std::map<Key, Type, Compare, Alloc> > generator;
		typedef map_accessor<std::map<Key, Type, Compare, Alloc> > accessor;
		
	public:
		typedef std::map<Key, Type, Compare, Alloc> container;
		
		map_wrapper(const map_wrapper& cp) :
			generator(cp.member), accessor(this->member) {}
		
		explicit map_wrapper(const container& cp) :
			generator(cp), accessor(this->member) {}
		
		explicit map_wrapper(const Compare& comp = Compare(), const Alloc& alloc = Alloc()) :
			generator(comp, alloc), accessor(this->member) {}
		
		template <class InIter>
		explicit map_wrapper(InIter first, InIter last,
			const Compare& comp = Compare(), const Alloc& alloc = Alloc()) :
			generator(first, last, comp, alloc), accessor(this->member) {}
		
		virtual ~map_wrapper() throw() {}
		
		map_wrapper& operator=(const map_wrapper& cp) {
			this->member = cp.member;
			return *this;
		}
		
		map_wrapper& operator=(const container& cp) {
			this->member = cp;
			return *this;
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  multimap_wrapper
	/* --------------------------------------------------------------------- */
	template <
		class Key,
		class Type,
		class Compare = std::less<Key>,
		class Alloc = std::allocator<Key>
	>
	class multimap_wrapper :
		protected comparable_container_generator<std::multimap<Key, Type, Compare, Alloc> >,
		public multimap_accessor<std::multimap<Key, Type, Compare, Alloc> > {
	private:
		typedef comparable_container_generator<std::multimap<Key, Type, Compare, Alloc> > generator;
		typedef multimap_accessor<std::multimap<Key, Type, Compare, Alloc> > accessor;
		
	public:
		typedef std::multimap<Key, Type, Compare, Alloc> container;
		
		multimap_wrapper(const multimap_wrapper& cp) :
			generator(cp.member), accessor(this->member) {}
		
		explicit multimap_wrapper(const container& cp) :
			generator(cp), accessor(this->member) {}
		
		explicit multimap_wrapper(const Compare& comp = Compare(), const Alloc& alloc = Alloc()) :
			generator(comp, alloc), accessor(this->member) {}
		
		template <class InIter>
		explicit multimap_wrapper(InIter first, InIter last,
			const Compare& comp = Compare(), const Alloc& alloc = Alloc()) :
			generator(first, last, comp, alloc), accessor(this->member) {}
		
		virtual ~multimap_wrapper() throw() {}
		
		multimap_wrapper& operator=(const multimap_wrapper& cp) {
			this->member = cp.member;
			return *this;
		}
		
		multimap_wrapper& operator=(const container& cp) {
			this->member = cp;
			return *this;
		}
	};
}

#endif // CLX_CONTAINER_WRAPPER_H
