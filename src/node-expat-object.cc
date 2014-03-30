
#include <stack>
#include <time.h>
#include <unistd.h>
#include <node.h>
#include "nan.h"
#include "parse.h"

#define CHUNK 1048576

using namespace v8;

inline void write_to_data(dynamic_data_t *data, char *source, size_t bytes)
{
  while ( (data->len + bytes) > data->allocated) {
    data->data = (char*)realloc((void*)data->data, data->allocated + CHUNK);
    data->allocated += CHUNK;
  }

  memcpy(
    (void*)&(data->data[data->len]),
    source,
    bytes
  );

  data->len += bytes;
}

void build(data_t *data, node::Buffer **returnBuffer)
{
  NanScope();

  dynamic_data_t string_data = {0, 0, NULL};
  dynamic_data_t binary_data = {0, 0, NULL};
  char next_node_type = 0;

  xml_node_t *curr_xml_node, *last_xml_node;
  xml_attr_t *curr_xml_attr, *last_xml_attr;
  int skip_attrs_and_children = 0;

  string_data.allocated = CHUNK;
  string_data.data = (char*)malloc(sizeof(char) * string_data.allocated);

  binary_data.allocated = CHUNK;
  binary_data.data = (char*)malloc(sizeof(char) * binary_data.allocated);

  // leave space for writing string data offset later
  binary_data.len = 4;

  // write number of nodes
  write_to_data(&binary_data, (char*)&(data->node_count), sizeof(size_t));

  curr_xml_node = data->node;

  do {
    if (!skip_attrs_and_children) {

      // indicate what comes next
      next_node_type = 1; // node
      write_to_data(&binary_data, &next_node_type, sizeof(char));

      // write node name length & name
      write_to_data(&binary_data, (char*)&(curr_xml_node->name_len), sizeof(size_t));
      write_to_data(&string_data, (char*)(curr_xml_node->name), curr_xml_node->name_len);

      // write number of attributes
      write_to_data(&binary_data, (char*)&(curr_xml_node->attribute_count), sizeof(size_t));

      // Add all attributes
      if ((curr_xml_attr = curr_xml_node->attributes)) {

        // add each attribute
        // [name_len, value_len, name, value]
        do {
          // Skip text attribute if this node has children
          write_to_data(&binary_data, (char*)&(curr_xml_attr->name_len),  sizeof(size_t));
          write_to_data(&binary_data, (char*)&(curr_xml_attr->value_len), sizeof(size_t));

          write_to_data(&string_data, (char*)(curr_xml_attr->name),  curr_xml_attr->name_len);
          write_to_data(&string_data, (char*)(curr_xml_attr->value), curr_xml_attr->value_len);

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

      next_node_type = 2; // deeper / child
      write_to_data(&binary_data, &next_node_type, sizeof(char));

    } else if (curr_xml_node->next) {

      last_xml_node = curr_xml_node;
      curr_xml_node = curr_xml_node->next;
      delete last_xml_node->name;
      delete last_xml_node;

      skip_attrs_and_children = 0;

    } else {
      next_node_type = 3; // parent
      write_to_data(&binary_data, &next_node_type, sizeof(char));

      last_xml_node = curr_xml_node;
      curr_xml_node = curr_xml_node->parent;
      delete last_xml_node->name;
      delete last_xml_node;

      skip_attrs_and_children = 1;
    }

  } while(curr_xml_node);

  delete data;

  next_node_type = 0; // end
  write_to_data(&binary_data, &next_node_type, sizeof(char));


  // write string data offset at the beginning of binary_data
  memcpy(
    (void*)binary_data.data,
    &(binary_data.len),
    sizeof(size_t)
  );

  // combine buffers
  write_to_data(&binary_data, string_data.data, string_data.len);
  free(string_data.data);

  *returnBuffer = node::Buffer::New(binary_data.data, binary_data.len);

  fprintf(stderr, "C binary_data.len: %u\n", binary_data.len);

  free(binary_data.data);
}

NAN_METHOD(convert) {
  int ret;
  size_t xml_str_len;
  char *xml_str = NULL;
  data_t *data = NULL;
  node::Buffer *buffer;
  double start, end;

  NanScope();

  Local<String>  xml  = args[0]->ToString();
  //Local<Object>  opts = args[1]->ToObject();
  NanCallback   *cb   = new NanCallback(args[2].As<Function>());

  xml_str = (char*)NanRawString(xml, Nan::ASCII, &xml_str_len, NULL, 0, v8::String::NO_OPTIONS);

  start = (double)clock()/CLOCKS_PER_SEC;
  ret = parse(xml_str, xml_str_len, &data);
  end = (double)clock()/CLOCKS_PER_SEC;

  printf("C time parse: %0.0lfms\n", (end - start) * 1000);

  delete xml_str;

  start = (double)clock()/CLOCKS_PER_SEC;
  build(data, &buffer);
  end = (double)clock()/CLOCKS_PER_SEC;

  printf("C time build: %.0lfms\n", (end - start) * 1000);

  Local<Value> argv[3] = {
    Local<Value>::New(Null()),
    Number::New(123.5),
    Local<Object>::New(buffer->handle_)
  };

  cb->Call(3, argv);

  NanReturnUndefined();
}

void register_module(Handle<Object> exports) {
  exports->Set(NanSymbol("convert"), FunctionTemplate::New(convert)->GetFunction());
}

NODE_MODULE(node_expat_object,  register_module);
