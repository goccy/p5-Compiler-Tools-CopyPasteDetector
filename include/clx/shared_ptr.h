/* ------------------------------------------------------------------------- */
/*
 *  shared_ptr.h
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
 *  Distributed under the Boost Software License, Version 1.0. (See
 *  accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Wed 13 Jan 2010 13:58:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SHARED_PTR_H
#define CLX_SHARED_PTR_H

#include "config.h"

#if defined(HAVE_TR1_MEMORY) || defined(HAS_TR1_MEMORY)

#include <tr1/memory>

namespace clx {
	using std::tr1::shared_ptr;
};

#elif defined(CLX_USE_BOOST)

#include <boost/shared_ptr.hpp>

namespace clx {
	using boost::shared_ptr;
}

#else

#include <algorithm>
#include <typeinfo>
#include "exception.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	/*
	 *  shared_ptr
	 *
	 *  The class is used when the system (compiler) does not have
	 *  std::tr1::shared_ptr
	 */
	/* --------------------------------------------------------------------- */
	template <class T>
	class shared_ptr {
	public:
		typedef T* pointer;
		typedef long count_type;
		
		// constructor and destructor
		shared_ptr() : p_(NULL) {}
		
		shared_ptr(const shared_ptr& cp) : p_(NULL) {
			p_ = cp.p_;
			if (cp.p_ != NULL) p_->increment();
		}
		
		template <class U>
		shared_ptr(const shared_ptr<U>& cp) : p_(NULL) {
			p_ = reinterpret_cast<shared_counter*>(cp.p_);
			if (cp.p_ != NULL) p_->increment();
		}
		
		explicit shared_ptr(T* cp) : p_(NULL) {
			if (cp != NULL) p_ = new shared_counter(cp);
		}
		
		template <class U>
		explicit shared_ptr(U* cp) : p_(NULL) {
			if (cp != NULL) {
				pointer tmp = dynamic_cast<pointer>(cp);
				if (tmp == NULL) throw std::bad_cast();
				p_ = new shared_counter(tmp);
			}
		}
		
		virtual ~shared_ptr() throw() { this->decrement(); }
		
		// operator
		shared_ptr& operator=(const shared_ptr& cp) {
			this->decrement();
			p_ = cp.p_;
			if (cp.p_ != NULL) p_->increment();
			return *this;
		}
		
		template <class U>
		shared_ptr& operator=(const shared_ptr<U>& cp) {
			this->decrement();
			p_ = reinterpret_cast<shared_counter*>(cp.p_);
			if (cp.p_ != NULL) p_->increment();
			return *this;
		}
		
		void reset() {
			this->decrement();
			p_ = NULL;
		}
		
		void swap(shared_ptr& other) { std::swap(p_, other.p_); }
		
		T& operator*() const { return *p_->get(); }
		pointer operator->() const { return (p_ != NULL) ? p_->get() : NULL; }
		pointer get() const { return (p_ != NULL) ? p_->get() : NULL; }
		
		count_type use_count() const { return (p_ != NULL) ? p_->count() : 0; }
		
		// comparison operator
		operator bool() const { return p_ != NULL; }
		
		template <class U>
		bool operator==(const shared_ptr<U>& other) const {
			return p_ == other.p_;
		}
		
		template <class U>
		bool operator!=(const shared_ptr<U>& other) const {
			return !((*this) == other);
		}
		
	private:
		class shared_counter {
		public:
			shared_counter(pointer p) : p_(p), count_(1) {}
			
			virtual ~shared_counter() {
				if (p_ != NULL) {
					delete p_;
					p_ = NULL;
				}
			}
			
			pointer get() { return p_; }
			count_type count() const { return count_; }
			count_type increment() { return ++count_; }
			count_type decrement() { return --count_; }
			
		private:
			pointer p_;
			count_type count_;
		};
		
		shared_counter* p_;
		
		void decrement() {
			if (p_ != NULL && p_->decrement() == 0) {
				delete p_;
				p_ = NULL;
			}
		}
	};
}

#endif

#endif // CLX_SHARED_PTR_H
