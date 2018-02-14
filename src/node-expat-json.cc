
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
//#define DEBUG


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

void to_json(data_t *data, str_t &json)
{
  dynamic_data_t json_d = {0, 0, NULL};
  xml_node_t *curr_xml_node;

  curr_xml_node = data->node;

  write_to_data(&json_d, "{", 1);
  to_json_rec(&json_d, curr_xml_node);
  write_to_data(&json_d, "}", 1);

  json.len = json_d.len;
  json.txt = json_d.data;
}

void convert(str_t &xml, str_t &json) {
  int ret;
  data_t *data = NULL;
  double start, end, total_start, total_end;

  total_start = (double)clock()/CLOCKS_PER_SEC;

  start = (double)clock()/CLOCKS_PER_SEC;
  ret = parse(xml, &data);
  end = (double)clock()/CLOCKS_PER_SEC;

  #ifdef DEBUG
  printf("C time parse: %0.0lfms\n", (end - start) * 1000);
  #endif

  free(xml.txt);
  xml.txt = NULL;
  xml.len = 0;

  start = (double)clock()/CLOCKS_PER_SEC;
  to_json(data, json);
  end = (double)clock()/CLOCKS_PER_SEC;

  #ifdef DEBUG
  printf("C time build: %.0lfms\n", (end - start) * 1000);
  #endif

  delete data;

  total_end = (double)clock()/CLOCKS_PER_SEC;

  #ifdef DEBUG
  printf("C time total: %.0lfms\n", (total_end - total_start) * 1000);
  #endif
}


class XMLToJSONConverter : public Nan::AsyncWorker {
  public:
    XMLToJSONConverter(Nan::Callback *callback, str_t &xml)
      : Nan::AsyncWorker(callback), xml(xml), json({0, NULL}) {}
    ~XMLToJSONConverter() {}

    // Executed inside the worker-thread.
    // It is not safe to access V8, or V8 data structures
    // here, so everything we need for input and output
    // should go on `this`.
    void Execute () {
      convert(xml, json);
      free(xml.txt);
    }

    // Executed when the async work is complete
    // this function will be run inside the main event loop
    // so it is safe to use V8 again
    void HandleOKCallback () {
      Nan::HandleScope scope;

      v8::Local<v8::Value> argv[] = {
        Nan::Null(),
        Nan::New<v8::String>(json.txt, json.len).ToLocalChecked()
      };
      free(json.txt);

      callback->Call(2, argv);
   }

  private:
    str_t xml;
    str_t json;
};

NAN_METHOD(setup) {
  str_t xml = { 0, NULL };

  Nan::Callback  *cb = new Nan::Callback(Nan::To<v8::Function>(info[2]).ToLocalChecked());
  Nan::Utf8String us(info[0]);

  xml.len = us.length();
  xml.txt = (char*)calloc(xml.len + 1, sizeof(char));
  strncpy(xml.txt, *us, xml.len * sizeof(char));

  AsyncQueueWorker(new XMLToJSONConverter(cb, xml));
}

NAN_MODULE_INIT(register_module) {
  Nan::Set(
    target, Nan::New<v8::String>("convert").ToLocalChecked(),
    Nan::GetFunction(Nan::New<v8::FunctionTemplate>(setup)).ToLocalChecked()
  );
}

NODE_MODULE(node_expat_json,  register_module);
