/* ------------------------------------------------------------------------- */
/*
 *  officex.h
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
#ifndef CLX_OFFICEX_H
#define CLX_OFFICEX_H

#include "config.h"
#include <string>
#include "unzip.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  is_docx
	/* --------------------------------------------------------------------- */
	inline bool is_docx(const std::basic_string<char>& path) {
		basic_unzip<char> in(path);
		if (!in.is_open()) return false;
		if (in.find("word/document.xml") == in.end()) return false;
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_docx
	/* --------------------------------------------------------------------- */
	inline bool is_docx(const char* path) {
		std::basic_string<char> tmp(path);
		return clx::is_docx(tmp);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_xlsx
	/* --------------------------------------------------------------------- */
	inline bool is_xlsx(const std::basic_string<char>& path) {
		basic_unzip<char> in(path);
		if (!in.is_open()) return false;
		if (in.find("xl/workbook.xml") == in.end()) return false;
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_xlsx
	/* --------------------------------------------------------------------- */
	inline bool is_xlsx(const char* path) {
		std::basic_string<char> tmp(path);
		return clx::is_xlsx(tmp);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_pptx
	/* --------------------------------------------------------------------- */
	inline bool is_pptx(const std::basic_string<char>& path) {
		basic_unzip<char> in(path);
		if (!in.is_open()) return false;
		if (in.find("ppt/presentation.xml") == in.end()) return false;
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_pptx
	/* --------------------------------------------------------------------- */
	inline bool is_pptx(const char* path) {
		std::basic_string<char> tmp(path);
		return clx::is_pptx(tmp);
	}
}

#endif // CLX_OFFICEX_H
