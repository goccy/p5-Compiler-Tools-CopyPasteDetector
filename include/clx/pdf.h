/* ------------------------------------------------------------------------- */
/*
 *  pdf.h
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
 *  Last-modified: Tue 18 Aug 2009 23:10:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_PDF_H
#define CLX_PDF_H

#include "config.h"
#include <istream>
#include <fstream>
#include <string>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  is_pdf
	/* --------------------------------------------------------------------- */
	inline bool is_pdf(std::basic_istream<char>& in) {
		std::basic_string<char> magic;
		if (!std::getline(in, magic)) return false;
		if (magic.find("%PDF-") == std::basic_string<char>::npos) return false;
		
		char tmp[8]; // %%EOF[[\r]\n]
		tmp[7] = 0;
		in.seekg(-7, std::ios_base::end);
		in.read(tmp, 7);
		if (in.fail() || in.gcount() < 7) return false;
		magic = tmp;
		if (magic.find("%%EOF") == std::basic_string<char>::npos) return false;
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_pdf
	/* --------------------------------------------------------------------- */
	inline bool is_pdf(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in);
		if (!fs.is_open()) return false;
		return clx::is_pdf(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_pdf
	/* --------------------------------------------------------------------- */
	inline bool is_pdf(const std::basic_string<char>& path) {
		return clx::is_pdf(path.c_str());
	}
}

#endif // CLX_PDF_H
