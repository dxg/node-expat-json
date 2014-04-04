
#include <stack>
#include <time.h>
#include <unistd.h>
#include <node.h>
#include "nan.h"
#include "parse.h"
#include <iostream>
#include <string>
#include <vector>

#define CHUNK 1048576

using namespace v8;

inline void write_to_data(dynamic_data_t *data, const char *source, size_t bytes, size_t chunk)
{
  while ( (data->len + bytes) > data->allocated) {
    data->data = (char*)realloc((void*)data->data, data->allocated + chunk);
    data->allocated += chunk;
  }

  memcpy(
    (void*)&(data->data[data->len]),
    source,
    bytes
  );

  data->len += bytes;
}

inline void write_to_data(dynamic_data_t *data, const char *source, size_t bytes)
{
  write_to_data(data, source, bytes, CHUNK);
}

typedef struct {
  std::string name;
  std::vector<xml_node_t*> items;

} xnode_hash_t;

void escape(dynamic_data_t *escaped, char *data, size_t len) {
  size_t a;
  char temp[8];

  if (len == 0) return;

  for (a = 0; a < len; a++) {
    switch (data[a]) {
      case '\\':
      case '"':
        write_to_data(escaped, "\\", 1, 1024);
        write_to_data(escaped, &(data[a]), 1, 1024);
        break;
      case '\b':
        write_to_data(escaped, "\\b", 2, 1024);
        break;
      case '\f':
        write_to_data(escaped, "\\f", 2, 1024);
        break;
      case '\n':
        write_to_data(escaped, "\\n", 2, 1024);
        break;
      case '\r':
        write_to_data(escaped, "\\r", 2, 1024);
        break;
      case '\t':
        write_to_data(escaped, "\\t", 2, 1024);
        break;
      default:
        if (data[a] < ' ') {
          snprintf(temp, 8, "%04x", data[a]);
          write_to_data(escaped, temp, strlen(temp), 1024);
        } else {
          write_to_data(escaped, &(data[a]), 1, 1024);
        }
    }

  }
}

void to_json_rec(dynamic_data_t *json, xml_node_t *node)
{
  size_t a, b;
  std::vector<xnode_hash_t*> hash;
  xnode_hash_t *new_hash_item;
  xml_attr *curr_attr;
  xml_attr *last_attr;

  xml_node_t *curr_xml_node = NULL;
  xml_node_t *curr_child_node = NULL;
  curr_xml_node = node;
  dynamic_data_t escaped = {0, 0, NULL};;

  // prepare hash of nodes
  do {
    for (a = 0; a < hash.size(); a++) {
      if (strcmp(hash[a]->name.c_str(), curr_xml_node->name) == 0) break;
    }
    if (a == hash.size()) {
      // not found
      new_hash_item = new xnode_hash_t;
      new_hash_item->name = curr_xml_node->name;
      new_hash_item->items.push_back(curr_xml_node);
      hash.push_back(new_hash_item);
    } else {
      // found
      hash[a]->items.push_back(curr_xml_node);
    }

  } while ((curr_xml_node = curr_xml_node->next));

  //printf(" ## node->name: %s, size: %d\n", node->name, hash.size());

  // write array of nodes grouped by name
  for (a = 0; a < hash.size(); a++) {
    write_to_data(json, "\"", 1);
    write_to_data(json, hash[a]->name.c_str(), hash[a]->name.size());
    write_to_data(json, "\": [", 4);

    // write individual nodes with same name
    //printf("    items size: %d\n", hash[a]->items.size());
    for (b = 0; b < hash[a]->items.size(); b++) {
      curr_xml_node = hash[a]->items[b];
      curr_child_node = curr_xml_node->children;
      curr_attr = curr_xml_node->attributes;

      // attributes
      write_to_data(json, "{", 1);
      if (curr_attr) {
        do {
          escaped.len = 0;
          escaped.allocated = 0;
          escaped.data = NULL;

          write_to_data(json, "\"", 1);
          write_to_data(json, curr_attr->name, curr_attr->name_len);
          write_to_data(json, "\": \"", 4);
          escape(&escaped, curr_attr->value, curr_attr->value_len);
          write_to_data(json, escaped.data, escaped.len);
          write_to_data(json, "\"", 1);

          free(escaped.data);

          if (curr_attr->next || curr_xml_node->children) {
            write_to_data(json, ",", 1);
          }

          last_attr = curr_attr;
          curr_attr = curr_attr->next;

          free(last_attr->name);
          free(last_attr->value);
          free(last_attr);

        } while (curr_attr);
      }

      // children
      if (curr_child_node) {
        to_json_rec(json, curr_child_node);
      }

      free(curr_xml_node->name);
      free(curr_xml_node);

      write_to_data(json, "}", 1);

      if (b != hash[a]->items.size() - 1) {
        write_to_data(json, ",", 1);
      }
    }

    write_to_data(json, "]", 1);

    if (a != hash.size() - 1) {
      write_to_data(json, ",", 1);
    }

    delete hash[a];
  }
}

void to_json(data_t *data, Persistent<String> *returnData)
{
  NanScope();
  Local<String> tmp_string;

  dynamic_data_t json = {0, 0, NULL};
  xml_node_t *curr_xml_node;

  curr_xml_node = data->node;
  write_to_data(&json, "{", 1);
  to_json_rec(&json, curr_xml_node);
  write_to_data(&json, "}", 1);

  tmp_string = String::New(json.data, json.len);
  NanAssignPersistent(String, *returnData, tmp_string);
}

NAN_METHOD(convert) {
  NanScope();

  int ret;
  size_t xml_str_len;
  char *xml_str = NULL;
  data_t *data = NULL;
  Persistent<String> json_string;
  double start, end, total_start, total_end;

  Local<String>  xml  = args[0]->ToString();
  //Local<Object>  opts = args[1]->ToObject();
  NanCallback   *cb   = new NanCallback(args[2].As<Function>());

  total_start = (double)clock()/CLOCKS_PER_SEC;
  xml_str = (char*)NanRawString(xml, Nan::ASCII, &xml_str_len, NULL, 0, v8::String::NO_OPTIONS);

  start = (double)clock()/CLOCKS_PER_SEC;
  ret = parse(xml_str, xml_str_len, &data);
  end = (double)clock()/CLOCKS_PER_SEC;

  printf("C time parse: %0.0lfms\n", (end - start) * 1000);

  delete xml_str;

  start = (double)clock()/CLOCKS_PER_SEC;
  to_json(data, &json_string);
  end = (double)clock()/CLOCKS_PER_SEC;

  printf("C time build: %.0lfms\n", (end - start) * 1000);
  delete data;

  Local<Value> argv[3] = {
    Local<Value>::New(Null()),
    Number::New(123.5),
    *json_string
  };

  total_end = (double)clock()/CLOCKS_PER_SEC;
  printf("C time total: %.0lfms\n", (total_end - total_start) * 1000);

  cb->Call(3, argv);

  NanReturnUndefined();
}

void register_module(Handle<Object> exports) {
  exports->Set(NanSymbol("convert"), FunctionTemplate::New(convert)->GetFunction());
}

NODE_MODULE(node_expat_json,  register_module);
