/* ------------------------------------------------------------------------- */
/*
 *  stats.h
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
 *  Last-modified: Fri 28 Nov 2008 02:20:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_STATS_H
#define CLX_STATS_H

#include "config.h"
#include <utility>
#include <cmath>
#include "lexical_cast.h"
#include "quantile.h"

namespace clx {
	namespace detail {
		/* ----------------------------------------------------------------- */
		//  basic_stats
		/* ----------------------------------------------------------------- */
		template <class Type>
		class basic_stats {
		public:
			typedef Type value_type;
			typedef unsigned int size_type;
			
			basic_stats() : sample_(0), exp_(0.0), var_(0.0), unbias_(0.0) {}
			virtual ~basic_stats() {}
			
			template <class InputIterator>
			basic_stats& assign(InputIterator first, InputIterator last) {
				value_type sum = 0.0;
				value_type power = 0.0;
				
				for (InputIterator p = first; p != last; ++p, ++sample_) {
					value_type tmp = lexical_cast<value_type>(*p);
					sum += tmp;
					power += std::pow(tmp, 2);
				}
				
				if (sample_ > 0) {
					exp_ = sum / static_cast<value_type>(sample_);
					var_ = power / static_cast<value_type>(sample_) - std::pow(exp_, 2);
					if (var_ < 0.0) var_ = 0.0;
					if (sample_ > 1) unbias_ = var_ * sample_ / static_cast<value_type>(sample_ - 1);
				}
				
				return *this;
			}
			
			size_type sample() const { return sample_; }
			value_type mean() const { return exp_; }
			value_type variance() const { return var_; }
			value_type unbiasvar() const { return unbias_; }
			
		private:
			size_type sample_;
			value_type exp_;
			value_type var_;
			value_type unbias_;
		};
	};
	
	/* --------------------------------------------------------------------- */
	//  mean
	/* --------------------------------------------------------------------- */
	template <class InputIterator>
	inline double mean(InputIterator first, InputIterator last) {
		detail::basic_stats<double> x;
		x.assign(first, last);
		return x.mean();
	}
	
	/* --------------------------------------------------------------------- */
	/*
	 *  variance
	 *
	 *  The variance function uses the following formula:
	 *  V[X] = E[X^2] - E[X]^2,
	 *  where V[X]/E[X] mean the variance/mean of X.
	 */
	/* --------------------------------------------------------------------- */
	template <class InputIterator>
	inline double variance(InputIterator first, InputIterator last) {
		detail::basic_stats<double> x;
		x.assign(first, last);
		return x.variance();
	}
	
	/* --------------------------------------------------------------------- */
	//  stddev
	/* --------------------------------------------------------------------- */
	template <class InputIterator>
	inline double stddev(InputIterator first, InputIterator last) {
		return std::sqrt(variance(first, last));
	}
	
	/* --------------------------------------------------------------------- */
	//  unbiasvar
	/* --------------------------------------------------------------------- */
	template <class InputIterator>
	inline double unbiasvar(InputIterator first, InputIterator last) {
		detail::basic_stats<double> x;
		x.assign(first, last);
		return x.unbiasvar();
	}
	
	/* --------------------------------------------------------------------- */
	//  confidence_interval
	/* --------------------------------------------------------------------- */
	template <class InputIterator, class Type>
	inline std::pair<double, double> confidence_interval(InputIterator first, InputIterator last, Type t) {
		detail::basic_stats<double> x;
		x.assign(first, last);
		double interval = t * std::sqrt(x.unbiasvar() / static_cast<double>(x.sample()));
		return std::pair<double, double>(x.mean() - interval, x.mean() + interval);
	}
	
	/* --------------------------------------------------------------------- */
	//  mse
	/* --------------------------------------------------------------------- */
	template <class InputIterator, class Type>
	inline double mse(InputIterator first, InputIterator last, Type correct) {
		double sum = 0.0;
		unsigned int n = 0;
		for (InputIterator p = first; p != last; ++p, ++n) {
			double tmp = lexical_cast<double>(*p) - static_cast<double>(correct);
			sum += std::pow(tmp, 2.0);
		}
		return sum / static_cast<double>(n);
	}
	
	/* --------------------------------------------------------------------- */
	//  rmse
	/* --------------------------------------------------------------------- */
	template <class InputIterator, class Type>
	inline double rmse(InputIterator first, InputIterator last, Type correct) {
		return std::sqrt(mse(first, last, correct));
	}
};

#endif // CLX_STATS_H
