#include <clx/md5.h>
#include <clx/base64.h>
#include <iostream>
#include <string>
#include <vector>
#ifdef __cplusplus
extern "C" {
#endif
#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
#define new_Array() (AV*)sv_2mortal((SV*)newAV())
#define new_Hash() (HV*)sv_2mortal((SV*)newHV())
#define new_String(s, len) sv_2mortal(newSVpv(s, len))
#define new_Int(u) sv_2mortal(newSVuv(u))
#define new_Ref(sv) sv_2mortal(newRV_inc((SV*)sv))
#define set(e) SvREFCNT_inc(e)

#ifdef __cplusplus
};
#endif
#include <pthread.h>
#define MAX_JOB_NUM 32
#define get_value(hash, key) *hv_fetchs(hash, key, strlen(key))
using namespace std;

class Stmt {
public:
	const char *src;
	const char *filename;
	int token_num;
	int indent;
	int block_id;
	int start_line;
	int end_line;
	Stmt(const char *src_, int token_num_, int indent_, int block_id_,
		 int start_line_, int end_line_) :
		src(src_), token_num(token_num_), indent(indent_), block_id(block_id_),
		start_line(start_line_), end_line(end_line_) {}
};

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
	vector<const char *> parents;
	DeparsedStmt(const char *hash_, const char *src_, const char *orig_, const char *file_,
				 int lines_,     int start_line_, int end_line_,
				 int indent_,    int block_id_,   int stmt_num_,
				 int token_num_) :
		hash(hash_), src(src_), orig(orig_), file(file_),
		lines(lines_), start_line(start_line_), end_line(end_line_),
		indent(indent_), block_id(block_id_), stmt_num(stmt_num_),
		token_num(token_num_) {}
};

typedef vector<Stmt *> Task;

static void add_stmt(vector<DeparsedStmt *> *deparsed_stmts, Stmt *stmt, string code, int stmt_num)
{
	const char *filename = stmt->filename;
	int token_num = stmt->token_num;
	int indent = stmt->indent;
	int block_id = stmt->block_id;
	int start_line = stmt->start_line;
	int end_line = stmt->end_line;
	int line_num = end_line - start_line;
	clx::md5 md5;
	DeparsedStmt *deparsed_stmt = new DeparsedStmt((new string(md5.encode(code).to_string()))->c_str(),
												   (new string(clx::base64::encode(code)))->c_str(),
												   (new string(code))->c_str(),
												   filename, (line_num > 0) ? line_num : 1,
												   start_line, end_line,
												   indent, block_id, stmt_num, token_num);
	vector<DeparsedStmt *> tmp_deparsed_stmts;
	for (size_t i = 0; i < deparsed_stmts->size(); i++) {
		DeparsedStmt *prev_stmt = deparsed_stmts->at(i);
		if (prev_stmt->stmt_num + 1 == stmt_num &&
			prev_stmt->indent == indent && prev_stmt->block_id == block_id) {
			string src = string(prev_stmt->orig) + "\n" + string(deparsed_stmt->orig);
			start_line = prev_stmt->start_line;
			line_num = end_line - start_line;
			const char *new_hash = (new string(md5.encode(src).to_string()))->c_str();
			prev_stmt->parents.push_back(new_hash);
			DeparsedStmt *added_stmt = new DeparsedStmt(new_hash,
														(new string(clx::base64::encode(src)))->c_str(),
														(new string(src))->c_str(),
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

static void set_deparsed_stmts(vector<DeparsedStmt *> *deparsed_stmts, Task *task,
							   const char *tmp_file_name, size_t stmts_size)
{
	size_t stmt_num = 0;
	for (size_t i = 0; i < stmts_size; i++) {
		Stmt *stmt = task->at(i);
		const char *src = stmt->src;
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
		add_stmt(deparsed_stmts, stmt, code, stmt_num);
		stmt_num++;
	}
	for (size_t i = 0; i < deparsed_stmts->size(); i++) {
		DeparsedStmt *stmt = deparsed_stmts->at(i);
		int start_line = stmt->start_line;
		int lines = stmt->lines;
		vector<const char *> parents;
		for (size_t j = 0; j < deparsed_stmts->size(); j++) {
			DeparsedStmt *another_stmt = deparsed_stmts->at(j);
			if ((another_stmt->start_line == start_line - (another_stmt->lines - lines)) &&
				(another_stmt->lines > stmt->lines)) {
				parents.push_back(another_stmt->hash);
			}
		}
		stmt->parents.insert(stmt->parents.end(), parents.begin(), parents.end());
	}
}

typedef struct _ThreadArgs {
	vector<Task *> tasks;
	size_t tasks_size;
	int thread_id;
	int hop_n;
} ThreadArgs;

static vector<DeparsedStmt *> total_deparsed_stmts[MAX_JOB_NUM];
static void *run(void *args_)
{
	ThreadArgs args = *(ThreadArgs *)args_;
	vector<Task *> tasks = args.tasks;
	int thread_id = args.thread_id;
	int hop_n = args.hop_n;
	size_t tasks_size = args.tasks_size;
	size_t tmp_file_name_size = 128;
	const char *tmp_file_name = (const char *)malloc(tmp_file_name_size);
	memset((char *)tmp_file_name, 0, tmp_file_name_size);
	snprintf((char *)tmp_file_name, tmp_file_name_size, "__copy_paste_detector%d.tmp", thread_id);
	vector<DeparsedStmt *> merged_deparsed_stmts;
	for (size_t i = thread_id; i <= tasks_size; i += hop_n) {
		Task *task = tasks.at(i);
		vector<DeparsedStmt *> deparsed_stmts;
		set_deparsed_stmts(&deparsed_stmts, task, tmp_file_name, task->size());
		merged_deparsed_stmts.insert(merged_deparsed_stmts.end(),
									 deparsed_stmts.begin(), deparsed_stmts.end());
	}
	/* Lock Free */
	total_deparsed_stmts[thread_id] = merged_deparsed_stmts;
	return NULL;
}

static Stmt *decode_stmt(pTHX, HV *stmt)
{
	char *src = SvPVX(get_value(stmt, "src"));
	int token_num = SvIVX(get_value(stmt, "token_num"));
	int indent = SvIVX(get_value(stmt, "indent"));
	int block_id = SvIVX(get_value(stmt, "block_id"));
	int start_line = SvIVX(get_value(stmt, "start_line"));
	int end_line = SvIVX(get_value(stmt, "end_line"));
	return new Stmt(src, token_num, indent, block_id, start_line, end_line);
}

static void setup_task(pTHX, Task *decoded_task, HV *task)
{
	const char *filename = SvPVX(get_value(task, "filename"));
	AV *stmts_ = (AV *)SvRV(get_value(task, "stmts"));
	SV **stmts = stmts_->sv_u.svu_array;
	if (stmts) {
		size_t stmts_size = av_len(stmts_);
		for (size_t i = 0; i < stmts_size; i++) {
			Stmt *stmt = decode_stmt(aTHX, (HV *)SvRV(stmts[i]));
			stmt->filename = filename;
			decoded_task->push_back(stmt);
		}
	}
}

AV *make_return_value(pTHX, vector<DeparsedStmt *> *deparsed_stmts)
{
	AV* ret  = new_Array();
	for (size_t j = 0; j < deparsed_stmts->size(); j++) {
		DeparsedStmt *stmt = deparsed_stmts->at(j);
		HV *hash = (HV*)new_Hash();
		hv_stores(hash, "hash", set(new_String(stmt->hash, strlen(stmt->hash) + 1)));
		hv_stores(hash, "src", set(new_String(stmt->src, strlen(stmt->src) + 1)));
		hv_stores(hash, "orig", set(new_String(stmt->orig, strlen(stmt->orig) + 1)));
		hv_stores(hash, "file", set(new_String(stmt->file, strlen(stmt->file) + 1)));
		hv_stores(hash, "lines", set(new_Int(stmt->lines)));
		hv_stores(hash, "start_line", set(new_Int(stmt->start_line)));
		hv_stores(hash, "end_line", set(new_Int(stmt->end_line)));
		hv_stores(hash, "indent", set(new_Int(stmt->indent)));
		hv_stores(hash, "block_id", set(new_Int(stmt->block_id)));
		hv_stores(hash, "stmt_num", set(new_Int(stmt->stmt_num)));
		hv_stores(hash, "token_num", set(new_Int(stmt->token_num)));
		AV* parents  = new_Array();
		for (size_t k = 0; k < stmt->parents.size(); k++) {
			const char *parent_hash = stmt->parents.at(k);
			av_push(parents, set(new_String(parent_hash, strlen(parent_hash))));
		}
		hv_stores(hash, "parent", set(new_Ref(parents)));
		av_push(ret, set(new_Ref(hash)));
	}
	return (AV *)new_Ref(ret);
}

MODULE = CopyPasteDetector		PACKAGE = CopyPasteDetector
PROTOTYPES: DISABLE

AV *
get_deparsed_stmts_by_xs_parallel(tasks_, job)
    AV *tasks_
	size_t job
CODE:
{
	size_t tasks_size = av_len(tasks_);
	SV **tasks = tasks_->sv_u.svu_array;
	vector<Task *> decoded_tasks;
	for (size_t i = 0; i <= tasks_size; i++) {
		HV *task = (HV *)SvRV(tasks[i]);
		decoded_tasks.push_back(new Task());
		setup_task(aTHX, decoded_tasks.at(i), task);
	}
	pthread_t th[job];
	size_t hop_n = job;
	ThreadArgs args[job];
	for (size_t i = 0; i < job; i++) {
		args[i].tasks = decoded_tasks;
		args[i].thread_id = i;
		args[i].hop_n = hop_n;
		args[i].tasks_size = tasks_size;
		pthread_create(&th[i], NULL, run, (void *)&args[i]);
	}
	for (size_t i = 0; i < job; i++) {
		pthread_join(th[i], NULL);
	}
	vector<DeparsedStmt *> merged_deparsed_stmts;
	for (size_t i = 0; i < job; i++) {
		merged_deparsed_stmts.insert(merged_deparsed_stmts.end(),
									 total_deparsed_stmts[i].begin(), total_deparsed_stmts[i].end());
	}
	RETVAL = make_return_value(aTHX, &merged_deparsed_stmts);
}
OUTPUT:
    RETVAL
