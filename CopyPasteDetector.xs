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

MODULE = CopyPasteDetector		PACKAGE = CopyPasteDetector		
PROTOTYPES: ENABLE

void
get_deparsed_op_list(source)
    const char *source
CODE:;
    char *arg[] = {NULL};
	size_t argc = 4;
	char **args = (char **)malloc(sizeof(char *) * 7);
	memset(args, 0, sizeof(char *) * 7);
	args[1] = "-MO=Deparse";
	args[2] = "-e";
	args[3] = source;
	PerlInterpreter *my_perl = perl_alloc();
	perl_construct(my_perl);
	perl_parse(my_perl, xs_init, argc, args, (char **)NULL);
	perl_run(my_perl);
