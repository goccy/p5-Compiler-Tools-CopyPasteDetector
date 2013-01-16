/* ------------------------------------------------------------------------- */
/*
 *  constrained_value.h
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
 *  The source code is derived from D. Ryan Stephens, Christopher Diggins,
 *  Janathan Turkanis, and Jeff Cogswell, ``C++ Cookbook,'' O'Reilly Media,
 *  Inc, 2006.
 *
 *  Last-modified: Mon 07 Jul 2008 10:24:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_CONSTRAINED_VALUE_H
#define CLX_CONSTRAINED_VALUE_H

#include "config.h"
#include <cstdlib>
#include <iostream>
#include "constrained_value_func.h"

namespace clx {
	template <class ConstrainedFunc>
	class constrained_value {
	public:
		typedef ConstrainedFunc func;
		typedef typename ConstrainedFunc::value_type value_type;
		
		constrained_value() : value_(func::default_value) {}
		constrained_value(const constrained_value& x) : value_(x.value_) {}
		explicit constrained_value(const value_type& x) { func::assign(value_, x); }
		virtual ~constrained_value() {}
		
		operator const value_type&() const { return value_; }
		const value_type& value() const { return value_; }
		
		// assignment operator
		constrained_value& operator=(const constrained_value& x) {
			this->assign(x.value_);
			return *this;
		}
		
		constrained_value& operator+=(const constrained_value& x) {
			this->assign(value_ + x.value_);
			return *this;
		}
		
		constrained_value& operator-=(const constrained_value& x) {
			this->assign(value_ - x.value_);
			return *this;
		}
		
		constrained_value& operator*=(const constrained_value& x) {
			this->assign(value_ * x.value_);
			return *this;
		}
		
		constrained_value& operator/=(const constrained_value& x) {
			this->assign(value_ / x.value_);
			return *this;
		}
		
		constrained_value& operator%=(const constrained_value& x) {
			this->assign(value_ % x.value_);
			return *this;
		}
		
		constrained_value& operator=(const value_type& x) {
			this->assign(x);
			return *this;
		}
		
		constrained_value& operator+=(const value_type& x) {
			this->assign(value_ + x);
			return *this;
		}
		
		constrained_value& operator-=(const value_type& x) {
			this->assign(value_ - x);
			return *this;
		}
		
		constrained_value& operator*=(const value_type& x) {
			this->assign(value_ * x);
			return *this;
		}
		
		constrained_value& operator/=(const value_type& x) {
			this->assign(value_ / x);
			return *this;
		}
		
		constrained_value& operator%=(const value_type& x) {
			this->assign(value_ % x);
			return *this;
		}
		
		constrained_value& operator>>=(int x) {
			this->assign(value_ >> x);
			return *this;
		}
		
		constrained_value& operator<<=(int x) {
			this->assign(value_ << x);
			return *this;
		}
		
		// unary operator 
		constrained_value operator-() { return constrained_value(-value_); }
		constrained_value operator+() { return constrained_value(+value_); }
		constrained_value operator!() { return constrained_value(!value_); }
		constrained_value operator~() { return constrained_value(~value_); }
		
		// binary operator
		friend constrained_value operator+(constrained_value x, const constrained_value& y) { return x += y; }
		friend constrained_value operator-(constrained_value x, const constrained_value& y) { return x -= y; }
		friend constrained_value operator*(constrained_value x, const constrained_value& y) { return x *= y; }
		friend constrained_value operator/(constrained_value x, const constrained_value& y) { return x /= y; }
		friend constrained_value operator%(constrained_value x, const constrained_value& y) { return x %= y; }
		
		friend constrained_value operator+(constrained_value x, const value_type& y) { return x += y; }
		friend constrained_value operator-(constrained_value x, const value_type& y) { return x -= y; }
		friend constrained_value operator*(constrained_value x, const value_type& y) { return x *= y; }
		friend constrained_value operator/(constrained_value x, const value_type& y) { return x /= y; }
		friend constrained_value operator%(constrained_value x, const value_type& y) { return x %= y; }
		friend constrained_value operator+(const value_type& y, constrained_value x) { return x += y; }
		friend constrained_value operator-(const value_type& y, constrained_value x) { return x -= y; }
		friend constrained_value operator*(const value_type& y, constrained_value x) { return x *= y; }
		friend constrained_value operator/(const value_type& y, constrained_value x) { return x /= y; }
		friend constrained_value operator%(const value_type& y, constrained_value x) { return x %= y; }
		
		friend constrained_value operator>>(constrained_value x, int y) { return x >>= y; }
		friend constrained_value operator<<(constrained_value x, int y) { return x <<= y; }
		
		// comparison operator
		friend bool operator<(const constrained_value& x, const constrained_value& y) { return x.value_ < y.value_; }
		friend bool operator>(const constrained_value& x, const constrained_value& y) { return x.value_ > y.value_; }
		friend bool operator<=(const constrained_value& x, const constrained_value& y) { return x.value_ <= y.value_; }
		friend bool operator>=(const constrained_value& x, const constrained_value& y) { return x.value_ >= y.value_; }
		friend bool operator==(const constrained_value& x, const constrained_value& y) { return x.value_ == y.value_; }
		friend bool operator!=(const constrained_value& x, const constrained_value& y) { return x.value_ != y.value_; }
		
		friend bool operator<(const constrained_value& x, const value_type& y) { return x.value_ < y; }
		friend bool operator>(const constrained_value& x, const value_type& y) { return x.value_ > y; }
		friend bool operator<=(const constrained_value& x, const value_type& y) { return x.value_ <= y; }
		friend bool operator>=(const constrained_value& x, const value_type& y) { return x.value_ >= y; }
		friend bool operator==(const constrained_value& x, const value_type& y) { return x.value_ == y; }
		friend bool operator!=(const constrained_value& x, const value_type& y) { return x.value_ != y; }
		friend bool operator<(const value_type& y, const constrained_value& x) { return x.value_ < y; }
		friend bool operator>(const value_type& y, const constrained_value& x) { return x.value_ > y; }
		friend bool operator<=(const value_type& y, const constrained_value& x) { return x.value_ <= y; }
		friend bool operator>=(const value_type& y, const constrained_value& x) { return x.value_ >= y; }
		friend bool operator==(const value_type& y, const constrained_value& x) { return x.value_ == y; }
		friend bool operator!=(const value_type& y, const constrained_value& x) { return x.value_ != y; }
		
	private:
		value_type value_;
		
		void assign(const value_type& x) {
			func::assign(value_, x);
		}
		

	};
};

#endif // CLX_CONSTRAINED_VALUE_H
