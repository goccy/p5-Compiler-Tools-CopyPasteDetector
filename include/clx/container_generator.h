/* ------------------------------------------------------------------------- */
/*
 *  container_generator.h
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
 *  Last-modified: Fri 09 Oct 2009 07:54:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_CONTAINER_GENERATOR_H
#define CLX_CONTAINER_GENERATOR_H

#include "config.h"
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>

namespace clx {
	/* --------------------------------------------------------------------- */
	/*
	 *  container_generator
	 *
	 *  The container_generator class is available when you use the
	 *  following STL container classes:
	 *   - std::vector
	 *   - std::deque
	 *   - std::list
	 *  If you use std::set/std::multiset/std::map/std::multimap,
	 *  use the comparable_container_class instead of the class.
	 */
	/* --------------------------------------------------------------------- */
	template <class Type>
	class container_generator {
	protected:
		typedef Type container;
		typedef typename Type::size_type size_type;
		typedef typename Type::value_type value_type;
		typedef typename Type::allocator_type allocator_type;
		
		container member;
		
		explicit container_generator(const container& cp) :
			member(cp) {}
		
		explicit container_generator(const allocator_type& alloc = allocator_type()) :
			member(alloc) {}
		
		explicit container_generator(size_type n, const value_type& value = value_type(),
			const allocator_type& alloc = allocator_type()) :
			member(n, value, alloc) {}
		
		template <class InIter>
		container_generator(InIter first, InIter last,
			const allocator_type& alloc = allocator_type()) :
			member(first, last, alloc) {}
	};
	
	/* --------------------------------------------------------------------- */
	/*
	 *  comparable_container_generator
	 *
	 *  When you use std::set (std::multiset) or std::map (std::multimap),
	 *  you use the comparable_container_generator class instead of
	 *  the container_generator class.
	 */
	/* --------------------------------------------------------------------- */
	template <class Type>
	class comparable_container_generator {
	protected:
		typedef Type container;
		typedef typename Type::key_type key_type;
		typedef typename Type::key_compare key_compare;
		typedef typename Type::allocator_type allocator_type;
		
		container member;
		
		explicit comparable_container_generator(const container& cp) :
			member(cp) {}
		
		explicit comparable_container_generator(const key_compare& comp = key_compare(),
			const allocator_type& alloc = allocator_type()) :
			member(comp, alloc) {}
		
		template <class InIter>
		comparable_container_generator(InIter first, InIter last,
			const key_compare& comp = key_compare(),
			const allocator_type& alloc = allocator_type()) :
			member(first, last, comp, alloc) {}
	};
}

#endif // CLX_CONTAINER_GENERATOR_H
