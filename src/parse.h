#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

extern "C" {
  #include <expat.h>
}

typedef struct xml_attr {
  char            *name;
  char            *value;

  struct xml_attr *next;

} xml_attr_t;

typedef struct xml_node {
  char            *name;
  struct xml_attr *attributes;
  struct xml_attr *attributes_last;
  struct xml_node *children;
  struct xml_node *children_last;

  struct xml_node *parent;
  struct xml_node *next;

} xml_node_t;

typedef struct {
  xml_node_t *node;
  xml_node_t *current_node; // internal use only when writing.

} data_t;

int parse(char *xml, size_t xml_len, data_t **data);
