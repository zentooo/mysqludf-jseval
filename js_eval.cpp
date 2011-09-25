#include <stdlib.h>
#include <string.h>
#include <mysql.h>

#include <v8.h>


using namespace v8;

extern "C" {

my_bool js_eval_init(UDF_INIT *initid, UDF_ARGS *args, char*message) {

    if ( args->arg_count != 1 ) {
        strcpy(message, "js_eval() can only accept one argument");
        return 1;
    }

    if ( args->arg_type[0] != STRING_RESULT ) {
        strcpy(message, "js_eval argument has to be a string.");
        return 1;
    }

    return 0;
}

char* js_eval(UDF_INIT *initid, UDF_ARGS* args, char *result_buf, unsigned long *res_length, char *null_value, char *error) {

    HandleScope handle_scope;

    Persistent<Context> context = Context::New();
    Context::Scope context_scope(context);

    //fprintf(stderr, "[mylog] input: %s\n", (char*)(args->args[0]));

    Handle<String> source = String::New((char*)(args->args[0]));
    Handle<Script> script = Script::Compile(source);

    TryCatch trycatch;
    Handle<Value> result = script->Run();

    if ( result.IsEmpty() ) {
        Handle<Value> exception = trycatch.Exception();
        String::AsciiValue exception_str(exception);

        //fprintf(stderr, "[mylog] exception: %s\n", *exception_str);

        strcpy(result_buf, *exception_str);
        *res_length = strlen(*exception_str);
    }
    else {
        String::AsciiValue ascii(result);

        //fprintf(stderr, "[mylog] output: %s\n", *ascii);

        strcpy(result_buf, *ascii);
        *res_length = strlen(*ascii);
    }

    context.Dispose();

    return result_buf;
}

}
