#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"

static void xs_init (pTHX);
EXTERN_C void boot_DynaLoader (pTHX_ CV* cv);

EXTERN_C void
xs_init(pTHX)
{
	char *file = __FILE__;
	newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
}

#define get_value(hash, key) *hv_fetchs(hash, key, strlen(key))

MODULE = CopyPasteDetector		PACKAGE = CopyPasteDetector		
PROTOTYPES: DISABLE


void
get_deparsed_op_list(stmts_)
    AV *stmts_
CODE:
{
	size_t size = av_len(stmts_);
	SV **stmts = stmts_->sv_u.svu_array;
	if (stmts) {
		size_t i = 0;
		for (; i <= size; i++) {
			//HV *stmt = (SvROK(stmts[i])) ? (HV *)stmts[i]->sv_u.svu_rv : (HV *)stmts[i];
			HV *stmt = SvRV(stmts[i]);
			char *src = SvPVX(get_value(stmt, "src"));
			int token_num = SvIVX(get_value(stmt, "token_num"));
			int indent = SvIVX(get_value(stmt, "indent"));
			int block_id = SvIVX(get_value(stmt, "block_id"));
			int start_line = SvIVX(get_value(stmt, "start_line"));
			int end_line = SvIVX(get_value(stmt, "end_line"));
			//fprintf(stderr, "s = %s\n", src);
			
		}
	}
}
