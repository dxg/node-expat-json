
#include <stack>
#include <time.h>
#include <unistd.h>
#include <node.h>
#include "nan.h"
#include "parse.h"

using namespace v8;

// This can't be in a separate thread because V8 doesn't
// play nice with threads.
void build(data_t *data, Persistent<Object> *returnObj)
{
  NanScope();

  xml_node_t *curr_xml_node, *last_xml_node;
  xml_attr_t *curr_xml_attr, *last_xml_attr;
  Local<Object> root;
  Local<Object> curr_obj;
  Local<Array>  curr_array;
  std::stack< Local<Object> > stack;
  int skip_attrs_and_children = 0;

  root = Object::New();

  curr_xml_node = data->node;
  stack.push(curr_obj);

  curr_obj = root;

  do {
    if (!skip_attrs_and_children) {
      // There may be multiple same-named elements, so they need to always go in an array.
      if (curr_obj->Has(NanSymbol(curr_xml_node->name))) {
        curr_array = Local<Array>::Cast( curr_obj->Get(NanSymbol(curr_xml_node->name)) );
      } else {
        curr_array = Array::New(0);
        curr_obj->Set(NanSymbol(curr_xml_node->name), curr_array);
      }
      curr_obj = Object::New();
      curr_array->Set(curr_array->Length(), curr_obj);

      // Add all attributes
      if ((curr_xml_attr = curr_xml_node->attributes)) {
        do {
          curr_obj->Set(NanSymbol(curr_xml_attr->name), String::New(curr_xml_attr->value));

          last_xml_attr = curr_xml_attr;
          curr_xml_attr = curr_xml_attr->next;
          delete last_xml_attr->name;
          delete last_xml_attr->value;
          delete last_xml_attr;

        } while(curr_xml_attr);
      }
    }

    if (!skip_attrs_and_children && curr_xml_node->children) {
      curr_xml_node = curr_xml_node->children;
      stack.push(curr_obj);
    } else if (curr_xml_node->next) {
      curr_obj = stack.top();

      last_xml_node = curr_xml_node;
      curr_xml_node = curr_xml_node->next;
      delete last_xml_node->name;
      delete last_xml_node;

      skip_attrs_and_children = 0;
    } else {
      last_xml_node = curr_xml_node;
      curr_xml_node = curr_xml_node->parent;
      delete last_xml_node->name;
      delete last_xml_node;

      curr_obj = stack.top();
      stack.pop();
      skip_attrs_and_children = 1;
    }

  } while(curr_xml_node);

  delete data;

  NanAssignPersistent(Object, *returnObj, root);
}

NAN_METHOD(convert) {
  int ret;
  size_t xml_str_len;
  char *xml_str = NULL;
  data_t *data = NULL;
  Persistent<Object> obj;
  double start, end;

  NanScope();

  Local<String>  xml  = args[0]->ToString();
  Local<Object>  opts = args[1]->ToObject();
  NanCallback   *cb   = new NanCallback(args[2].As<Function>());

  xml_str = (char*)NanRawString(xml, Nan::ASCII, &xml_str_len, NULL, 0, v8::String::NO_OPTIONS);

  start = (double)clock()/CLOCKS_PER_SEC;
  ret = parse(xml_str, xml_str_len, &data);
  end = (double)clock()/CLOCKS_PER_SEC;

  printf("C time parse: %0.3lfs\n", end - start);

  delete xml_str;

  // Why so slow?
  // It turns out that setting V8 object properties in C++ land is
  // actually quite slow, compared to setting them in JS land.
  // V8 does fancy stuff in it's JSON.parse implementation by
  // building the raw JSObjects. Us commoners are not allowed to
  // do this, as the required classes are internal and not exposed.
  start = (double)clock()/CLOCKS_PER_SEC;
  build(data, &obj);
  end = (double)clock()/CLOCKS_PER_SEC;

  printf("C time build: %0.3lfs\n", end - start);

  Local<Value> argv[] = {
    Local<Value>::New(Null()),
    Number::New(123.5),
    *obj
  };

  cb->Call(3, argv);

  NanReturnUndefined();
}

void register_module(Handle<Object> exports) {
  exports->Set(NanSymbol("convert"), FunctionTemplate::New(convert)->GetFunction());
}

NODE_MODULE(node_expat_object,  register_module);
