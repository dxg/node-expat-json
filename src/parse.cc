#include "parse.h"

void add_attr_to_current(data_t *data, const char *name, const char *value)
{
  xml_attr_t *new_attr;
  size_t name_len, value_len;

  name_len  = strlen(name);
  value_len = strlen(value);

  //printf("  @[%s] %d, %d\n", name, name_len, value_len);
  new_attr = new xml_attr_t(); // zero initialize
  new_attr->name  = new char[name_len  + 1];
  new_attr->value = new char[value_len + 1];

  strcpy(new_attr->name,  name);
  strcpy(new_attr->value, value);

  if (data->current_node->attributes == NULL) {
    data->current_node->attributes = new_attr;
  } else {
    data->current_node->attributes_last->next = new_attr;
  }
  data->current_node->attributes_last = new_attr;
}

void add_child_to_current(data_t *data, const char *name)
{
  xml_node_t *new_node;
  size_t name_len;

  name_len = strlen(name);

  new_node = new xml_node_t(); // zero initialize
  new_node->name = new char[name_len + 1];
  new_node->parent = data->current_node;

  strcpy(new_node->name, name);

  if (data->node == NULL) {
    data->node = new_node;
    data->current_node = new_node;
  } else {
    if (data->current_node->children == NULL) {
      data->current_node->children = new_node;
    } else {
      data->current_node->children_last->next = new_node;
    }
    data->current_node->children_last = new_node;
    data->current_node = new_node;
  }
}

void start_element(void *data, const char *element_name, const char **attrs)
{
  size_t i;

  //printf("--> <%s>\n", element_name);

  add_child_to_current((data_t*)data, element_name);

  for(i = 0; ; i+=2) {
    if(attrs[i] == NULL) break;

    add_attr_to_current((data_t*)data, attrs[i], attrs[i+1]);
  }
}

void end_element(void *data, const char *el)
{
  data_t *d;
  d = (data_t*)data;

  //printf("<-- <%s>\n", el);

  d->current_node = d->current_node->parent;
}

// pastes parts of the node together
void char_data(void *data, const XML_Char *s, int len)
{
  if (len == 1 && s[0] == '\n') return;

  size_t sz = sizeof(XML_Char) * len;
  char str[sz + sizeof(XML_Char)];

  memcpy(str, s, sz);
  str[sz - sizeof(XML_Char)] = 0;

  add_attr_to_current((data_t*)data, "$t", str);
}

int parse(char *xml, size_t xml_len, data_t **data)
{
  XML_Parser parser;
  int ret = 0;
  double start, end;

  *data = new data_t(); // zero initialize

  parser = XML_ParserCreate("US-ASCII");

  XML_SetUserData(parser, (void*)*data);
  XML_SetElementHandler(parser, start_element, end_element);
  XML_SetCharacterDataHandler(parser, char_data);

  ret = XML_Parse(parser, xml, xml_len, XML_TRUE);

  XML_ParserFree(parser);

  if (ret == XML_STATUS_ERROR) {
    printf("Error: %d, %s\n", XML_GetErrorCode(parser), XML_ErrorString(XML_GetErrorCode(parser)));
    return -1;
  } else {
    return 0;
  }
}
