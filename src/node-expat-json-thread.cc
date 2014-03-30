
#include <stack>
#include <time.h>
#include <unistd.h>
#include <node.h>
#include "nan.h"
#include "parse.h"

using namespace v8;

class XMLWorker : public NanAsyncWorker {
 public:
  XMLWorker(NanCallback *callback, str_t xml) : NanAsyncWorker(callback), xml(xml) {}
  ~XMLWorker() {}

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  void Execute () {
    int ret;
    double start, end;

    start = (double)clock()/CLOCKS_PER_SEC;
    ret = parse(&xml, &json);
    end = (double)clock()/CLOCKS_PER_SEC;
    printf("C time parse: %0.3lfs\n", end - start);
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    NanScope();

    delete xml.txt;

    Local<Value> argv[] = {
      Local<Value>::New(Null()),
      String::New(json.txt)
      //String::New(json.txt, json.len)
    };

    callback->Call(2, argv);
  };

 private:
  data_t *data = NULL;
  str_t   xml;
  str_t   json;
};

NAN_METHOD(convert) {
  str_t str;

  NanScope();

  Local<String>  xml  = args[0]->ToString();
  Local<Object>  opts = args[1]->ToObject();
  NanCallback   *cb   = new NanCallback(args[2].As<Function>());

  str.txt = (char*)NanRawString(xml, Nan::ASCII, &(str.len), NULL, 0, v8::String::NO_OPTIONS);

  NanAsyncQueueWorker(new XMLWorker(cb, str));

  NanReturnUndefined();
}

void register_module(Handle<Object> exports) {
  exports->Set(NanSymbol("convert"), FunctionTemplate::New(convert)->GetFunction());
}

NODE_MODULE(node_expat_json,  register_module);
