/* ------------------------------------------------------------------------- */
/*
 *  sockmanager.h
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
 *  Last-modified: Fri 30 Jul 2010 22:01:10 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SOCKMANAGER_H
#define CLX_SOCKMANAGER_H

#include "config.h"
#include <map>
#include "shared_ptr.h"
#include "socket.h"
#include "timer.h"
#include "time_duration.h"
#include "exception.h"

namespace clx {
	template <int Type, int Family, int Protocol> class basic_sockmanager;
	namespace detail {
		template <class Socket>
		class socket_less {
		public:
			bool operator()(const shared_ptr<Socket>& x, const shared_ptr<Socket>& y) const {
				return x->socket() < y->socket();
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_sockhandler
		/* ----------------------------------------------------------------- */
		template <
			int Type,
			int Family,
			int Protocol = 0
		>
		class sockhandler_base {
		public:
			typedef basic_rawsocket<Type, Family, Protocol> rawsocket_type;
			typedef basic_sockmanager<Type, Family, Protocol> sockmanager_type;
			
			sockhandler_base() {}
			virtual ~sockhandler_base() {}
			virtual bool run(rawsocket_type* s, sockmanager_type& sm) = 0;
		};
	};
	
	
	/* --------------------------------------------------------------------- */
	/*
	 *  basic_sockmanager
	 *
	 *  The basic_sockmanager class monitors received packets of the joined
	 *  socket classes. If the class detects packets, the class calles
	 *  the handler related with the target socket of the packets.
	 *
	 *  The basic_sockmanager class is implemented by using select(2)
	 *  system call.
	 */
	/* --------------------------------------------------------------------- */
	template <
		int Type,
		int Family,
		int Protocol = 0
	>
	class basic_sockmanager {
	public:
		typedef basic_rawsocket<Type, Family, Protocol> rawsocket_type;
		typedef detail::sockhandler_base<Type, Family, Protocol> basehandler_type;
		typedef shared_ptr<rawsocket_type> socket_ptr;
		typedef shared_ptr<basehandler_type> handler_ptr;
		typedef std::map<socket_ptr, handler_ptr, detail::socket_less<rawsocket_type> > rfdset;
		typedef typename rfdset::size_type size_type;
		typedef typename rfdset::iterator iterator;
		typedef typename rfdset::const_iterator const_iterator;
		typedef struct timeval timeval_type;
		
		basic_sockmanager() : rfds_(), stop_(false), interval_(-1.0) {}
		
		virtual ~basic_sockmanager() throw() {}
		
		template <class Handler>
		bool add(rawsocket_type* s, Handler h) {
			if (s == NULL) return false;
			shared_ptr<rawsocket_type> ps(s);
			
			sockhandler<Handler>* p = new sockhandler<Handler>(h);
			if (p == NULL) throw std::bad_alloc();
			shared_ptr<basehandler_type> ph(p);
			rfds_[ps] = ph;
			return true;
		}
		
		bool remove(rawsocket_type* s) {
			for (iterator pos = rfds_.begin(); pos != rfds_.end(); ++pos) {
				if (pos->first->socket() == s->socket()) {
					rfds_.erase(pos);
					return true;
				}
			}
			return false;
		}
		
		void start() {
			stop_ = false;
			
			clx::timer t;
			while (!stop_ && !rfds_.empty()) {
				fd_set sysrfds;
				FD_ZERO(&sysrfds);
				for (iterator pos = rfds_.begin(); pos != rfds_.end(); ++pos) {
					FD_SET(pos->first->socket(), &sysrfds);
				}
				
				// calculate timeout value.
				clx::shared_ptr<timeval_type> tv;
				if (interval_ > 0.0) {
					double timeout = interval_ - t.total_elapsed();
					if (timeout < 0.0) break;
					timeval_type* p = new timeval_type();
					if (p == NULL) throw std::bad_alloc();
					tv = clx::shared_ptr<timeval_type>(p);
					tv->tv_sec = static_cast<int>(timeout);
					tv->tv_usec = static_cast<int>((timeout - tv->tv_sec) * 1e+06);
				}
				
				int n = select(FD_SETSIZE, &sysrfds, NULL, NULL, tv.get());
				if (n < 0) throw clx::socket_error("select");
				else if (n == 0) break; // timeout
				
				for (iterator pos = rfds_.begin(); pos != rfds_.end(); ++pos) {
					if (pos->first->pending() > 0 || FD_ISSET(pos->first->socket(), &sysrfds)) {
						if (!pos->second->run(pos->first.get(), *this)) {
							rfds_.erase(pos++);
						}
						if (pos == rfds_.end()) break;
					}
				}
			}
		}
		
		void start(const timeval_type& t) {
			interval_ = t.tv_sec + t.tv_usec * 1e-06;
			this->start();
		}
		
		void start(double sec) {
			interval_ = sec;
			this->start();
		}
		
		void stop() { stop_ = true; }
		
		void reset() {
			stop_ = false;
			interval_ = -1.0;
		}
		
		size_type size() const { return rfds_.size(); }
		bool empty() const { return rfds_.empty(); }
		
		iterator begin() { return rfds_.begin(); }
		iterator end() { return rfds_.end(); }
		rawsocket_type* socket(iterator pos) { return pos->first.get(); }
		
		const_iterator begin() const { return rfds_.begin(); }
		const_iterator end() const { return rfds_.end(); }
		const rawsocket_type* socket(const_iterator pos) const { return pos->first.get(); }
		
	private:
		rfdset rfds_;
		bool stop_;
		double interval_;
		
		template <class F>
		class sockhandler : public basehandler_type {
		public:
			typedef typename basehandler_type::rawsocket_type rawsocket_type;
			typedef typename basehandler_type::sockmanager_type sockmanager_type;
			
			sockhandler(F f) : f_(f) {}
			virtual ~sockhandler() {}
			virtual bool run(rawsocket_type* s, sockmanager_type& sm) { return f_(s, sm); }
		private:
			F f_;
		};
	};
}

#endif // CLX_SOCKMANAGER_H
