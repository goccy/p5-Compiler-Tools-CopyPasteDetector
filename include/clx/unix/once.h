/* ------------------------------------------------------------------------- */
/*
 *  unix/once.h
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
 *  Distributed under the Boost Software License, Version 1.0. (See
 *  accompanying file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 *  Last-modified: Wed 27 May 2009 14:35:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UNIX_ONCE_H
#define CLX_UNIX_ONCE_H

#include <climits>
#include <cstdlib>
#include <pthread.h>
#include "system_error.h"

#define CLX_ONCE_INIT 0

namespace clx {
	typedef unsigned long once_flag;
	
	namespace detail {
		struct global_condition {
			once_flag flag;
			pthread_mutex_t mutex;
			pthread_cond_t cond;
			
			global_condition() :
				flag(ULONG_MAX), mutex(), cond() {
				int res = pthread_mutex_init(&mutex, NULL);
				if (res) throw system_error("pthread_mutex_init");
				res = pthread_cond_init(&cond, NULL);
				if (res) throw system_error("pthread_cond_init");
			}
			
			~global_condition() {
				pthread_mutex_destroy(&mutex);
				pthread_cond_destroy(&cond);
			}
		};
		
		inline global_condition& get_global_condition() {
			static global_condition cv;
			return cv;
		}
		
		inline pthread_once_t& get_epoch_tss_once_flag() {
			static pthread_once_t flag = PTHREAD_ONCE_INIT;
			return flag;
		}
		
		inline pthread_key_t& get_epoch_tss_key() {
			static pthread_key_t key;
			return key;
		}
		
		inline void delete_epoch_tss_data(void* data) {
			free(data);
		}
		
		inline void create_epoch_tss_key() {
			pthread_key_t& key = get_epoch_tss_key();
			pthread_key_create(&key, delete_epoch_tss_data);
		}
		
		inline once_flag& get_once_per_thread_epoch() {
			pthread_once_t& flag = get_epoch_tss_once_flag();
			pthread_key_t& key = get_epoch_tss_key();
			pthread_once(&flag, create_epoch_tss_key);
			void* data = pthread_getspecific(key);
			if (!data) {
				data = std::malloc(sizeof(once_flag));
				pthread_setspecific(key, data);
				*static_cast<once_flag*>(data) = ULONG_MAX;
			}
			return *static_cast<once_flag*>(data);
		}
	};
	
	/* --------------------------------------------------------------------- */
	/*
	 *  call_once
	 *
	 *  Based on Mike Burrows fast_pthread_once algorithm as described in
	 *  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2444.html
	 */
	/* --------------------------------------------------------------------- */
	template <class Functor>
	inline void call_once(once_flag& flag, Functor f) {
		static once_flag const uninitialized_flag = CLX_ONCE_INIT;
		static once_flag const being_initialized = uninitialized_flag + 1;
		once_flag const epoch = flag;
		once_flag& this_thread_epoch = detail::get_once_per_thread_epoch();
		
		detail::global_condition& inf = detail::get_global_condition();
		if (epoch < this_thread_epoch) {
			pthread_mutex_lock(&inf.mutex);
			while (flag <= being_initialized) {
				if (flag == uninitialized_flag) {
					flag = being_initialized;
					pthread_mutex_unlock(&inf.mutex);
					f();
					pthread_mutex_lock(&inf.mutex);
					
					--inf.flag;
					flag = inf.flag;
					pthread_cond_broadcast(&inf.cond);
				}
				else {
					while (flag == being_initialized) {
						pthread_cond_wait(&inf.cond, &inf.mutex);
					}
				}
			}
			
			this_thread_epoch = inf.flag;
			pthread_mutex_unlock(&inf.mutex);
		}
	}
}

#endif // CLX_UNIX_ONCE_H
