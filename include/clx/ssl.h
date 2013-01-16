/* ------------------------------------------------------------------------- */
/*
 *  ssl.h
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
 *  Copyright (c) 1998-2008 The OpenSSL Project.  All rights reserved.
 *  Copyright (c) 1995-1998 Eric Young (eay@cryptsoft.com)
 *  All rights reserved.
 *  
 *  The source code is derived from OpenSSL project.
 *  (See the following URL: http://www.openssl.org/source/license.html)
 *
 *  Last-modified: Wed 02 Jun 2010 04:26:03 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SSL_H
#define CLX_SSL_H

#include "config.h"
#include <ctime>
#include <string>
#include <stdexcept>
#include <new>

#include "socket.h"
#include "sockaddress.h"
#include "sockstream.h"
#include "sockmanager.h"
#include "shared_ptr.h"
#include "random.h"
#include "timer.h"
#include "exception.h"

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#ifdef CLX_ENABLE_AUTOLINK
#if defined(_MSC_VER) && (_MSC_VER >= 1200) || defined(__BORLANDC__)
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif
#endif // CLX_ENABLE_AUTOLINK

namespace clx {
	namespace detail {
		class ssl_init {
		public:
			ssl_init() {
				SSL_load_error_strings();
				SSL_library_init();
			}
			
			~ssl_init() {
				ERR_free_strings();
			}
			
		private:
			ssl_init(const ssl_init& cp);
			ssl_init& operator=(const ssl_init& cp);
		};
		
		static ssl_init ssl_init_;
	} // namespace detail
	
	/* --------------------------------------------------------------------- */
	//  verify_error
	/* --------------------------------------------------------------------- */
	class verify_error : public std::runtime_error {
	public:
		typedef char char_type;
		typedef std::string string_type;
		typedef std::stringstream sstream_type;
		
		static const int nbuf = 1024;
		
		explicit verify_error(int status) :
			std::runtime_error("certification failed"),
			errno_(status), message_() {}
		
		virtual ~verify_error() throw() {}
		
		virtual const char_type* what() {
			sstream_type tmp;
			tmp << std::runtime_error::what() << ": status 0x" << std::hex << errno_;
			message_ = tmp.str();
			return message_.c_str();
		}
		
		int code() const { return errno_; }
		
	private:
		int errno_;
		string_type message_;
	};
	
	namespace ssl {
		/* ----------------------------------------------------------------- */
		//  SSL/TLS version
		/* ----------------------------------------------------------------- */
		enum { sslv2 = 0x01, sslv3, tlsv1, sslv23 };
		enum { pem = SSL_FILETYPE_PEM, asn1 = SSL_FILETYPE_ASN1 };
		
		template <int Version, int Family> class basic_acceptor;
		/* ----------------------------------------------------------------- */
		//  basic_socket
		/* ----------------------------------------------------------------- */
		template <
			int Version,
			int Family
		>
		class basic_socket : public basic_rawsocket<SOCK_STREAM, Family, 0> {
		private:
			typedef basic_rawsocket<SOCK_STREAM, Family, 0> super;
		public:
			typedef SSL handler;
			typedef basic_sockaddress<Family, IPPROTO_TCP> address_type;
			typedef char char_type;
			typedef typename std::basic_string<char> string_type;
			
			/* ----------------------------------------------------------------- */
			//  constructor
			/* ----------------------------------------------------------------- */
			basic_socket() : super(), p_(), addr_(), connect_(false) {}
			
			/* ----------------------------------------------------------------- */
			//  constructor
			/* ----------------------------------------------------------------- */
			basic_socket(const basic_socket& cp) :
				super(cp), p_(cp.p_), addr_(cp.addr_), connect_(cp.connect_) {}
			
			/* ----------------------------------------------------------------- */
			//  constructor
			/* ----------------------------------------------------------------- */
			template <class Socket>
			explicit basic_socket(const Socket& cp) :
				super(cp), p_(), addr_(cp.address()), connect_(cp.is_connect()) {
				this->init();
			}
			
			/* ----------------------------------------------------------------- */
			/*
			 *  constructor
			 *
			 *  The constructor is called when making a new client ssl socket.
			 */
			/* ----------------------------------------------------------------- */
			basic_socket(const char_type* host, int port) :
				super(), p_(), addr_(host, port) {
				this->connect_and_handshake();
			}
			
			/* ----------------------------------------------------------------- */
			/*
			 *  constructor
			 *
			 *  The constructor is called when making a new client ssl socket.
			 */
			/* ----------------------------------------------------------------- */
			basic_socket(const string_type& host, int port) :
				super(), p_(), addr_(host, port) {
				this->connect_and_handshake();
			}
			
			/* ----------------------------------------------------------------- */
			/*
			 *  constructor
			 *
			 *  The constructor is called when making a new server ssl socket.
			 */
			/* ----------------------------------------------------------------- */
			basic_socket(socket_int s, const address_type& addr) :
				super(s), p_(), addr_(addr) {
				this->init();
				connect_ = true;
			}
			
			/* ----------------------------------------------------------------- */
			//  destructor
			/* ----------------------------------------------------------------- */
			virtual ~basic_socket() throw() {}
			
			/* ----------------------------------------------------------------- */
			//  assignment operator.
			/* ----------------------------------------------------------------- */
			basic_socket& operator=(const basic_socket& cp) {
				addr_ = cp.addr_;
				this->assign(static_cast<super>(cp));
				p_ = cp.p_;
				return *this;
			}
			
			/* ----------------------------------------------------------------- */
			//  connect
			/* ----------------------------------------------------------------- */
			basic_socket& connect(const char_type* host, int port) {
				addr_.assign(host, port);
				return this->connect_and_handshake();
			}
			
			/* ----------------------------------------------------------------- */
			//  connect
			/* ----------------------------------------------------------------- */
			basic_socket& connect(const string_type& host, int port) {
				return this->connect(host.c_str(), port);
			}
			
			/* ----------------------------------------------------------------- */
			//  handshake
			/* ----------------------------------------------------------------- */
			basic_socket& handshake() {
				if (this->is_handshake()) return *this;
				this->create_ssl(client);
				
				RAND_poll();
				mt19937 engine(static_cast<unsigned long>(std::time(NULL)));
				random<unsigned short> rng(engine);
				while (RAND_status() == 0) {
					unsigned short value = rng() % 65536;
					RAND_seed(&value, sizeof(value));
				}
				
				char err[1024];
				if (SSL_connect(p_->ssl()) <= 0) {
					std::memset(err, '\0', sizeof(err));
					ERR_error_string(ERR_get_error(), err);
					throw std::runtime_error(err);
				}
				p_->handshake() = true;
				
				if (p_->verify()) {
					int status = SSL_get_verify_result(p_->ssl());
					if (status != X509_V_OK) throw verify_error(status);
				}
				
				return *this;
			}
			
			/* ----------------------------------------------------------------- */
			//  handshake
			/* ----------------------------------------------------------------- */
			basic_socket& handshake(const char_type* cert, const char_type* priv, int type = pem) {
				if (this->is_handshake()) return *this;
				this->create_ssl(server);
				
				SSL_use_certificate_file(p_->ssl(), cert, type);
				SSL_use_PrivateKey_file(p_->ssl(), priv, type);
				
				char err[1024];
				if (SSL_accept(p_->ssl()) <= 0) {
					std::memset(err, '\0', sizeof(err));
					ERR_error_string(ERR_get_error(), err);
					throw std::runtime_error(err);
				}
				p_->handshake() = true;
				
				return *this;
			}
			
			/* ----------------------------------------------------------------- */
			//  send
			/* ----------------------------------------------------------------- */
			int send(const char_type* src, int n) {
				if (!p_ || !p_->ssl()) return -1;
				return SSL_write(p_->ssl(), src, n);
			}
			
			/* ----------------------------------------------------------------- */
			//  send
			/* ----------------------------------------------------------------- */
			int send(const string_type& src) {
				return this->send(src.c_str(), static_cast<int>(src.size()));
			}
			
			/* ----------------------------------------------------------------- */
			//  recv
			/* ----------------------------------------------------------------- */
			int recv(char_type* dest, int n) {
				if (!p_ || !p_->ssl()) return -1;
				return SSL_read(p_->ssl(), dest, n);
			}
			
			/* ----------------------------------------------------------------- */
			//  recv
			/* ----------------------------------------------------------------- */
			template <std::size_t N>
			int recv(char_type (&dest)[N]) {
				return this->recv(dest, N);
			}
			
			/* ----------------------------------------------------------------- */
			//  close
			/* ----------------------------------------------------------------- */
			virtual void close() {
				super::close();
				if (p_) p_->close();
				connect_ = false;
			}
			
			/* ----------------------------------------------------------------- */
			//  pending
			/* ----------------------------------------------------------------- */
			virtual int pending() const {
				if (!p_ || !p_->ssl()) return 0;
				return SSL_pending(p_->ssl());
			}
			
			/* ----------------------------------------------------------------- */
			//  verify_locations
			/* ----------------------------------------------------------------- */
			bool verify_locations(const char_type* file, const char_type* dir = NULL) {
				if (!p_ || !p_->ctx() || SSL_CTX_load_verify_locations(p_->ctx(), file, dir) == 0) return false;
				p_->verify() = true;
				return true;
			}
			
			/* ----------------------------------------------------------------- */
			//  verify_locations
			/* ----------------------------------------------------------------- */
			bool verify_locations(const string_type& file, const string_type& dir = string_type()) {
				if (file.empty() && dir.empty()) return false;
				else if (dir.empty()) return this->verify_locations(file.c_str(), NULL);
				else if (file.empty()) return this->verify_locations(NULL, dir.c_str());
				else return this->verify_locations(file.c_str(), dir.c_str());
			}
			
			/* ----------------------------------------------------------------- */
			//  access methods.
			/* ----------------------------------------------------------------- */
			bool is_connect() const { return connect_; }
			bool is_handshake() const { return p_ ? p_->handshake() : false; }
			const address_type& address() const { return addr_; }
			handler* ssl() { return p_ ? p_->ssl() : NULL; }
			const handler* ssl() const { return p_ ? p_->ssl() : NULL; }
			
		private:
			typedef SSL_CTX ctx_type;
			enum { client = 0x10, server = 0x20 };
			
			class ssl_impl {
			public:
				typedef ctx_type* ctx_pointer;
				typedef handler* handle_pointer;
				
				ssl_impl() : ctx_(NULL), ssl_(NULL), handshake_(false), verify_(false) {}
				~ssl_impl() { this->close(); }
				
				ctx_pointer& ctx() { return ctx_; }
				handle_pointer& ssl() { return ssl_; }
				bool& handshake() { return handshake_; }
				bool& verify() { return verify_; }
				
				void close() {
					if (ssl_ != NULL) {
						SSL_shutdown(ssl_);
						SSL_free(ssl_);
						ssl_ = NULL;
					}
					
					if (ctx_ != NULL) {
						SSL_CTX_free(ctx_);
						ctx_ = NULL;
					}
					
					handshake_ = false;
					verify_ = false;
				}
				
			private:
				ctx_pointer ctx_;
				handle_pointer ssl_;
				bool handshake_;
				bool verify_;
			};
			
			shared_ptr<ssl_impl> p_;
			address_type addr_;
			bool connect_;
			
			basic_socket& init() {
				if (!this->is_open()) this->open();
				if (!this->is_open()) throw socket_error("open");
				if (!p_) p_ = shared_ptr<ssl_impl>(new ssl_impl());
				return *this;
			}
			
			basic_socket& create_ssl(int mode) {
				SSL_METHOD *(*f)() = NULL;
				switch (Version) {
				case sslv2:
					if (mode == client) this->assign_ssl_method(SSLv2_client_method, f);
					else this->assign_ssl_method(SSLv2_server_method, f);
					break;
				case sslv3:
					if (mode == client) this->assign_ssl_method(SSLv3_client_method, f);
					else this->assign_ssl_method(SSLv3_server_method, f);
					break;
				case tlsv1:
					if (mode == client) this->assign_ssl_method(TLSv1_client_method, f);
					else this->assign_ssl_method(TLSv1_server_method, f);
					break;
				case sslv23:
				default:
					if (mode == client) this->assign_ssl_method(SSLv23_client_method, f);
					else this->assign_ssl_method(SSLv23_server_method, f);
					break;
				}				
				if (!p_->ctx()) p_->ctx() = SSL_CTX_new(f());
				if (!p_->ctx()) throw std::runtime_error("SSL_CTX_new failed");
				
				char err[1024];
				if (!p_->ssl()) p_->ssl() = SSL_new(p_->ctx());
				if (!p_->ssl() || SSL_set_fd(p_->ssl(), this->socket()) == 0) {
					std::memset(err, '\0', sizeof(err));
					ERR_error_string(ERR_get_error(), err);
					throw std::runtime_error(err);
				}
				
				return *this;
			}
			
			// for client ssl socket
			basic_socket& connect_and_handshake() {
				this->init();
				if (!this->is_connect()) {
					if (::connect(this->socket(), (struct sockaddr*)addr_.data(), addr_.size()) == -1) {
						throw clx::socket_error("connect");
					}
					connect_ = true;
				}
				return this->handshake();
			}
			
			template <class Source, class Type>
			void assign_ssl_method(const Source* src, Type*& dest) {
				dest = reinterpret_cast<Type*>(const_cast<Source*>(src));
			}
			
			template <class Source, class Type>
			void assign_ssl_method(Source* src, Type*& dest) {
				dest = reinterpret_cast<Type*>(src);
			}
		};
		
		/* ----------------------------------------------------------------- */
		/*
		 *  basic_acceptor
		 *
		 *  The basic_acceptor class creates a welcome socket which
		 *  accepts/rejects the client connecting request. When accepting
		 *  a new client request, the class generates the basic_socket
		 *  instance for the accepted client.
		 */
		/* ----------------------------------------------------------------- */
		template <
			int Version,
			int Family
		>
		class basic_acceptor : public basic_rawsocket<SOCK_STREAM, Family> {
		private:
			typedef basic_rawsocket<SOCK_STREAM, Family> super;
		public:
			typedef basic_sockaddress<Family, IPPROTO_TCP> address_type;
			typedef char char_type;
			typedef typename std::basic_string<char> string_type;
			
			enum { nmax = 100 };
			
			// constructor and destructor
			basic_acceptor() : super(), nlisten_(nmax) {}
			
			explicit basic_acceptor(int port, int n = nmax) :
				super(), addr_(port), nlisten_(nmax) {
				this->start();
			}
			
			virtual ~basic_acceptor() throw() {}
			
			// operator
			basic_acceptor& open(int port, int n = nmax) {
				addr_.assign(port);
				nlisten_ = n;
				return this->start();
			}
			
			basic_socket<Version, Family> accept(const char_type* cert, const char_type* priv, int type = pem) {
				socket_int s = -1;
				address_type addr;
				typename address_type::inet_type tmp;
				socklen_type len = sizeof(tmp);
				
				for (int foo = 0; foo < 10; ++foo) {
					s = ::accept(this->socket(), (struct sockaddr*)&tmp, &len);
					if (s == -1) clx::sleep(1.0);
					else break;
				}
				if (s == -1) throw socket_error("accept");
				
				addr.assign(tmp);
				basic_socket<Version, Family> client(s, addr);
				client.handshake(cert, priv, pem);
				
				return client;
			}
			
			basic_socket<Version, Family> accept(const string_type& cert, const string_type& priv, int type = pem) {
				return this->accept(cert.c_str(), priv.c_str(), type);
			}
			
		private:
			address_type addr_;
			int nlisten_;
			
			basic_acceptor& start() {
				if (static_cast<int>(super::open()) < 0) throw socket_error("socket");
				if (::bind(this->socket(), (struct sockaddr*)addr_.data(), addr_.size()) == -1) {
					throw socket_error("bind");
				}
				if (::listen(this->socket(), nlisten_) == -1) throw socket_error("listen");
				return *this;
			}
		};
		
		/* ----------------------------------------------------------------- */
		//  port
		/* ----------------------------------------------------------------- */
		inline int port(const char* service) {
			return clx::port(service, "tcp");
		}
		
		inline int port(const std::basic_string<char>& service) {
			return clx::ssl::port(service.c_str());
		}
		
		typedef basic_rawsocket<SOCK_STREAM, AF_INET, 0> rawsocket;
		typedef basic_socket<sslv23, AF_INET> socket;
		typedef basic_acceptor<sslv23, AF_INET> acceptor;
		typedef basic_sockaddress<AF_INET, IPPROTO_TCP> sockaddress;
		typedef basic_sockbuf<socket> sockbuf;
		typedef basic_sockstream<socket> sockstream;
		typedef basic_sockmanager<SOCK_STREAM, AF_INET, 0> sockmanager;
	}
}

#endif // CLX_SSL_H
