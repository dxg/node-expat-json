#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

extern "C" {
  #include <expat.h>
}

typedef struct {
  size_t  len;
  char   *txt;
} str_t;

typedef struct {
  size_t  len;
  size_t  allocated;
  char   *data;
} dynamic_data_t;

typedef struct xml_attr {
  char            *name;
  char            *value;
  size_t           name_len;
  size_t           value_len;

  struct xml_attr *next;

} xml_attr_t;

typedef struct xml_node {
  char            *name;
  size_t           name_len;
  size_t           attribute_count;
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
  size_t      node_count;
  size_t      attribute_count;

} data_t;

int parse(str_t &xml, data_t **data);
