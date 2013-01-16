/* ------------------------------------------------------------------------- */
/*
 *  container_accessor.h
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
 *  Last-modified: Sat 07 Nov 2009 01:34:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_ACCESSOR_H
#define CLX_ACCESSOR_H

#include "config.h"
#include "noncopyable.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  container_accessor
	/* --------------------------------------------------------------------- */
	template <class Type>
	class container_accessor : public noncopyable<container_accessor<Type> > {
	public:
		typedef typename Type::size_type size_type;
		typedef typename Type::value_type value_type;
		typedef typename Type::iterator iterator;
		typedef typename Type::const_iterator const_iterator;
		typedef typename Type::reverse_iterator reverse_iterator;
		typedef typename Type::const_reverse_iterator const_reverse_iterator;
		
		explicit container_accessor(Type& v) : v_(v) {}
		virtual ~container_accessor() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  cast operators
		/* ----------------------------------------------------------------- */
		operator Type&() { return v_; }
		operator const Type&() const { return v_; }
		
		/* ----------------------------------------------------------------- */
		//  iterator-related methods
		/* ----------------------------------------------------------------- */
		iterator begin() { return v_.begin(); }
		const_iterator begin() const { return v_.begin(); }
		iterator end() { return v_.end(); }
		const_iterator end() const { return v_.end(); }
		reverse_iterator rbegin() { return v_.rbegin(); }
		const_reverse_iterator rbegin() const { return v_.rbegin(); }
		reverse_iterator rend() { return v_.rend(); }
		const_reverse_iterator rend() const { return v_.rend(); }
		
		/* ----------------------------------------------------------------- */
		//  size-related methods
		/* ----------------------------------------------------------------- */
		size_type size() const { return v_.size(); }
		size_type max_size() const { return v_.max_size(); }
		bool empty() const { return v_.empty(); }
		
		/* ----------------------------------------------------------------- */
		//  assign/insert
		/* ----------------------------------------------------------------- */
		template <class InIter>
		void assign(InIter first, InIter last) {
			v_.assign(first, last);
		}
		
		void assign(size_type n, const value_type& u) { v_.assign(n, u); }
		
		iterator insert(iterator pos, const value_type& x) { return v_.insert(pos, x); }
		
		template <class InIter>
		void insert(iterator pos, InIter first, InIter last) {
			v_.insert(pos, first, last);
		}
		
		/* ----------------------------------------------------------------- */
		//  erase/clear
		/* ----------------------------------------------------------------- */
		iterator erase(iterator pos) { return v_.erase(pos); }
		iterator erase(iterator first, iterator last) { return v_.erase(first, last); }
		void clear() { v_.clear(); }
		
	private:
		Type& v_;
	};
	
	/* --------------------------------------------------------------------- */
	//  random_accessor
	/* --------------------------------------------------------------------- */
	template <class Type>
	class random_accessor : public container_accessor<Type> {
	public:
		typedef typename Type::size_type size_type;
		typedef typename Type::value_type value_type;
		typedef typename Type::reference reference;
		typedef typename Type::const_reference const_reference;
		typedef typename Type::iterator iterator;
		typedef typename Type::const_iterator const_iterator;
		
		explicit random_accessor(Type& v) :
			container_accessor<Type>(v), v_(v) {}
		
		virtual ~random_accessor() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  resize
		/* ----------------------------------------------------------------- */
		void resize(size_type n, value_type c = value_type()) { v_.resize(n, c); }
		
		/* ----------------------------------------------------------------- */
		//  reference methods
		/* ----------------------------------------------------------------- */
		reference operator[](size_type pos) { return v_[pos]; }
		const_reference operator[](size_type pos) const { return v_[pos]; }
		reference at(size_type pos) { return v_.at(pos); }
		const_reference at(size_type pos) const { return v_.at(pos); }
		reference front() { return v_.front(); }
		const_reference front() const { return v_.front(); }
		reference back() { return v_.back(); }
		const_reference back() const { return v_.back(); }
		
		/* ----------------------------------------------------------------- */
		//  insert-related methods
		/* ----------------------------------------------------------------- */
		void insert(iterator pos, size_type n, const value_type& x) { v_.insert(pos, n, x); }
		void push_back(const value_type& x) { v_.push_back(x); }
		
		/* ----------------------------------------------------------------- */
		//  pop_back
		/* ----------------------------------------------------------------- */
		void pop_back() { v_.pop_back(); }
		
	private:
		Type& v_;
	};
	
	/* --------------------------------------------------------------------- */
	//  vector_accessor
	/* --------------------------------------------------------------------- */
	template <class Type>
	class vector_accessor : public random_accessor<Type> {
	public:
		typedef typename Type::size_type size_type;
		typedef typename Type::value_type value_type;
		typedef typename Type::reference reference;
		typedef typename Type::const_reference const_reference;
		
		vector_accessor(Type& v) : random_accessor<Type>(v), v_(v) {}
		virtual ~vector_accessor() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  size-related methods
		/* ----------------------------------------------------------------- */
		size_type capacity() const { return v_.capacity(); }
		void reserve(size_type n) { v_.reserve(n); }
		
	private:
		Type& v_;
	};
	
	/* --------------------------------------------------------------------- */
	//  deque_accessor
	/* --------------------------------------------------------------------- */
	template <class Type>
	class deque_accessor : public random_accessor<Type> {
	public:
		typedef typename Type::size_type size_type;
		typedef typename Type::value_type value_type;
		typedef typename Type::reference reference;
		typedef typename Type::const_reference const_reference;
		
		deque_accessor(Type& v) : random_accessor<Type>(v), v_(v) {}
		virtual ~deque_accessor() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  push_front/pop_front
		/* ----------------------------------------------------------------- */
		void push_front(const value_type& x) { v_.push_front(x); }
		void pop_front() { v_.pop_front(); }
		
	private:
		Type& v_;
	};
	
	/* --------------------------------------------------------------------- */
	//  set_accessor
	/* --------------------------------------------------------------------- */
	template <class Type>
	class set_accessor : public container_accessor<Type> {
	public:
		typedef typename Type::size_type size_type;
		typedef typename Type::value_type value_type;
		typedef typename Type::key_type key_type;
		typedef typename Type::iterator iterator;
		typedef typename Type::const_iterator const_iterator;
		typedef typename Type::key_compare key_compare;
		typedef typename Type::value_compare value_compare;
		
		explicit set_accessor(Type& v) :
			container_accessor<Type>(v), v_(v) {}
		
		virtual ~set_accessor() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  insert
		/* ----------------------------------------------------------------- */
		iterator insert(const value_type& x) { return v_.insert(x); }
		
		/* ----------------------------------------------------------------- */
		//  erase
		/* ----------------------------------------------------------------- */
		size_type erase(const key_type& x) { return v_.erase(x); }
		
		/* ----------------------------------------------------------------- */
		//  find-related methods
		/* ----------------------------------------------------------------- */
		const_iterator find(const key_type& x) const { return v_.find(x); }
		size_type count(const key_type& x) const { return v_.count(x); }
		const_iterator lower_bound(const key_type& x) const { return v_.lower_bound(x); }
		const_iterator upper_bound(const key_type& x) const { return v_.upper_bound(x); }
		std::pair<const_iterator, const_iterator> equal_range(const key_type& x) const { return v_.equal_range(x); }
		
		/* ----------------------------------------------------------------- */
		//  get comparison object
		/* ----------------------------------------------------------------- */
		key_compare key_comp() const { return v_.key_comp(); }
		value_compare value_comp() const { return v_.value_comp(); }
		
	private:
		Type& v_;
	};
	
	/* --------------------------------------------------------------------- */
	//  multimap_accessor
	/* --------------------------------------------------------------------- */
	template <class Type>
	class multimap_accessor : public set_accessor<Type> {
	public:
		typedef typename Type::key_type key_type;
		typedef typename Type::mapped_type mapped_type;
		typedef typename Type::iterator iterator;
		
		explicit multimap_accessor(Type& v) :
			set_accessor<Type>(v), v_(v) {}
		
		virtual ~multimap_accessor() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  find-related methods
		/* ----------------------------------------------------------------- */
		iterator find(const key_type& x) { return v_.find(x); }
		iterator lower_bound(const key_type& x) { return v_.lower_bound(x); }
		iterator upper_bound(const key_type& x) { return v_.upper_bound(x); }
		std::pair<iterator, iterator> equal_range(const key_type& x) const { return v_.equal_range(x); }
		
	private:
		Type& v_;
	};
	
	/* --------------------------------------------------------------------- */
	//  map_accessor
	/* --------------------------------------------------------------------- */
	template <class Type>
	class map_accessor : public multimap_accessor<Type> {
	public:
		typedef typename Type::key_type key_type;
		typedef typename Type::mapped_type mapped_type;
		
		map_accessor(Type& v) :
			multimap_accessor<Type>(v), v_(v) {}
		
		virtual ~map_accessor() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  [] operator
		/* ----------------------------------------------------------------- */
		mapped_type& operator[](const key_type& x) { return v_[x]; }
		
	private:
		Type& v_;
	};
	
	/* --------------------------------------------------------------------- */
	//  list_accessor
	/* --------------------------------------------------------------------- */
	template <class Type>
	class list_accessor : public container_accessor<Type> {
	public:
		typedef typename Type::size_type size_type;
		typedef typename Type::value_type value_type;
		typedef typename Type::reference reference;
		typedef typename Type::const_reference const_reference;
		typedef typename Type::iterator iterator;
		typedef typename Type::const_iterator const_iterator;
		
		explicit list_accessor(Type& v) :
			container_accessor<Type>(v), v_(v) {}
		
		virtual ~list_accessor() throw() {}
		
		/* ----------------------------------------------------------------- */
		//  resize
		/* ----------------------------------------------------------------- */
		void resize(size_type n, value_type c = value_type()) { v_.resize(n, c); }
		
		/* ----------------------------------------------------------------- */
		//  reference methods
		/* ----------------------------------------------------------------- */
		reference front() { return v_.front(); }
		const_reference front() const { return v_.front(); }
		reference back() { return v_.back(); }
		const_reference back() const { return v_.back(); }
		
		/* ----------------------------------------------------------------- */
		//  insert-related methods
		/* ----------------------------------------------------------------- */
		void insert(iterator pos, size_type n, const value_type& x) { v_.insert(pos, n, x); }
		void push_front(const value_type& x) { v_.push_front(); }
		void push_back(const value_type& x) { v_.push_back(x); }
		
		/* ----------------------------------------------------------------- */
		//  pop_front/pop_back
		/* ----------------------------------------------------------------- */
		void pop_front() { v_.pop_back(); }
		void pop_back() { v_.pop_back(); }
		
		/* ----------------------------------------------------------------- */
		//  other methods
		/* ----------------------------------------------------------------- */
		void remove(const value_type& x) { v_.remove(x); }
		
		template <class Predicate>
		void remove_if(Predicate pred) {
			v_.remove_if(pred);
		}
		
		void unique() { v_.unique(); }
		
		template <class BinaryPredicate>
		void unique(BinaryPredicate binary_pred) {
			v_.unique(binary_pred);
		}
		
		void sort() { v_.sort(); }
		
		template <class Compare>
		void sort(Compare comp) {
			v_.sort(comp);
		}
		
		void reverse() { v_.reverse(); }
		
	private:
		Type& v_;
	};
}

#endif // CLX_ACCESSABLE_H
