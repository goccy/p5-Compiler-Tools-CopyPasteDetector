/* ------------------------------------------------------------------------- */
/*
 *  ip.h
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
 *  Last-modified: Sun 27 Jul 2008 12:12:03 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_IP_H
#define CLX_IP_H

#include "socket.h"

struct iphdr {
	u_int8_t ihl:4;
	u_int8_t version:4;
	u_int8_t tos;
	
	u_int16_t tot_len;
	u_int16_t id;
	u_int16_t frag_off;
	
	u_int8_t ttl;
	u_int8_t protocol;
	
	u_int16_t check;
	
	unsigned long saddr;
	unsigned long daddr;
};

namespace clx {
	namespace ip {
		/* ----------------------------------------------------------------- */
		//  checksum
		/* ----------------------------------------------------------------- */
		inline u_int16_t checksum(u_int16_t* buf, int size) {
			unsigned long sum = 0;
			
			while (size > 1) {
				sum += *buf;
				++buf;
				size -= 2;
			}
			if (size) sum += *(u_int8_t*)buf;
			
			sum = (sum & 0xffff) + (sum >> 16);
			sum = (sum & 0xffff) + (sum >> 16);
			
			return ~static_cast<u_int16_t>(sum);
		}
	}
}

#endif // CLX_IP_H
