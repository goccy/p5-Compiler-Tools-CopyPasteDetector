#ifdef __cplusplus
extern "C" {
#endif
#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
#ifdef __cplusplus
};
#endif
#include <clx/md5.h>
#include <clx/base64.h>
#include <iostream>
#include <string>
#include <vector>

#define get_value(hash, key) *hv_fetchs(hash, key, strlen(key))
using namespace std;
class DeparsedStmt {
public:
	const char *hash;
	const char *src;
	const char *orig;
	const char *file;
	int lines;
	int start_line;
	int end_line;
	int indent;
	int block_id;
	int stmt_num;
	int token_num;
	vector<const char *> parent;
	DeparsedStmt(const char *hash_, const char *src_, const char *orig_, const char *file_,
				 int lines_,     int start_line_, int end_line_,
				 int indent_,    int block_id_,   int stmt_num_,
				 int token_num_) :
		hash(hash_), src(src_), orig(orig_), file(file_),
		lines(lines_), start_line(start_line_), end_line(end_line_),
		indent(indent_), block_id(block_id_), stmt_num(stmt_num_),
		token_num(token_num_) {}
};

static void add_stmt(pTHX, vector<DeparsedStmt *> *deparsed_stmts, HV *stmt, string code, const char *filename, int stmt_num)
{
	int token_num = SvIVX(get_value(stmt, "token_num"));
	int indent = SvIVX(get_value(stmt, "indent"));
	int block_id = SvIVX(get_value(stmt, "block_id"));
	int start_line = SvIVX(get_value(stmt, "start_line"));
	int end_line = SvIVX(get_value(stmt, "end_line"));
	int line_num = end_line - start_line;
	clx::md5 md5;
	DeparsedStmt *deparsed_stmt = new DeparsedStmt(md5.encode(code).to_string().c_str(),
												   clx::base64::encode(code).c_str(), code.c_str(),
												   (char *)filename, (line_num > 0) ? line_num : 1,
												   (int)start_line, (int)end_line,
												   (int)indent, (int)block_id, (int)stmt_num, (int)token_num);
	vector<DeparsedStmt *> tmp_deparsed_stmts;
	for (size_t i = 0; i < deparsed_stmts->size(); i++) {
		DeparsedStmt *prev_stmt = deparsed_stmts->at(i);
		if (prev_stmt->stmt_num + 1 == stmt_num &&
			prev_stmt->indent == indent && prev_stmt->block_id == block_id) {
			string src = string(prev_stmt->orig) + "\n" + string(deparsed_stmt->orig);
			start_line = prev_stmt->start_line;
			line_num = end_line - start_line;
			const char *new_hash = md5.encode(src).to_string().c_str();
			prev_stmt->parent.push_back(new_hash);
			DeparsedStmt *added_stmt = new DeparsedStmt(new_hash, clx::base64::encode(src).c_str(),
														src.c_str(),
														filename, (line_num > 0) ? line_num : 1,
														start_line, end_line,
														indent, block_id, stmt_num,
														prev_stmt->token_num + token_num);
			tmp_deparsed_stmts.push_back(added_stmt);
		}
	}
	deparsed_stmts->push_back(deparsed_stmt);
	deparsed_stmts->insert(deparsed_stmts->end(), tmp_deparsed_stmts.begin(), tmp_deparsed_stmts.end());
}

MODULE = CopyPasteDetector		PACKAGE = CopyPasteDetector
PROTOTYPES: DISABLE


void
get_deparsed_op_list(stmts_)
    AV *stmts_
CODE:
{
	size_t size = av_len(stmts_);
	const char *filename = "test.pl";
	SV **stmts = stmts_->sv_u.svu_array;
	const char *tmp_file_name = "__copy_paste_detector.tmp";
	if (stmts) {
		vector<DeparsedStmt *> deparsed_stmts;
		for (size_t stmt_num = 0; stmt_num <= size; stmt_num++) {
			HV *stmt = (HV *)SvRV(stmts[stmt_num]);
			char *src = SvPVX(get_value(stmt, "src"));
			char cmd_buf[256] = {0};
			FILE *fp = fopen(tmp_file_name, "w");
			if (!fp) {
				perror("fopen");
				exit(EXIT_FAILURE);
			}
			fprintf(fp, "%s", src);
			fclose(fp);
			snprintf(cmd_buf, 256, "perl -MList::Util -MCarp -MO=Deparse %s 2> /dev/null", tmp_file_name);
			fp = popen(cmd_buf, "r");
			char read_buf[128] = {0};
			if (!fp) {
				perror("popen");
				exit(EXIT_FAILURE);
			}
			string code = "";
			while ((fgets(read_buf , 128 , fp)) != NULL) {
				code += string(read_buf);
			}
			pclose(fp);
			if (code == "" || code == "'???';\n" || code == ";\n") continue;
			code.erase(code.size() - 1);
			add_stmt(aTHX, &deparsed_stmts, stmt, code, filename, stmt_num);
		}
	}
}
