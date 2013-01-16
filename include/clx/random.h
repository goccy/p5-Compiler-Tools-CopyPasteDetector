/* ------------------------------------------------------------------------- */
/*
 *  random.h
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
 *  Last-modified: Fri 30 Jul 2010 22:15:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_RANDOM_H
#define CLX_RANDOM_H

#include "config.h"
#include "mt19937.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  random
	/* --------------------------------------------------------------------- */
	template <
		class Type,
		class Engine = mt19937
	>
	class random {
	public:
		typedef Type value_type;
		typedef Engine engine_type;
		
		explicit random(engine_type e = engine_type()) : engine_(e) {}
		virtual ~random() {}
		
		value_type operator()() {
			value_type dest;
			engine_(dest);
			return dest;
		}
		
		engine_type& engine() { return engine_; }
		const engine_type& engine() const { return engine_; }
		
	private:
		engine_type engine_;
	};
}

#endif // CLX_RANDOM_H
