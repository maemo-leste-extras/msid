#ifndef MSID_SUPER_SEARCH_PLUGIN
#define MSID_SUPER_SEARCH_PLUGIN

#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "../search_iface.h"

#ifdef __cplusplus
extern "C" {
#endif

// C++ class definition (no extern "C")
class super_search : public msid_search_plugin
{
  public :

  super_search() { tree = NULL; }
  ~super_search();

  GSList *search_for_sid (const char *needle, gpointer data);
  unsigned int init(); // TODO - load local database here

  private :
  GTree *tree;
};

// Only wrap C-style functions in extern "C"
extern "C" void* create() {
  return new super_search;
}

extern "C" void destroy (void *plug) {
  delete (super_search *) plug;
}

#ifdef __cplusplus
}
#endif

#endif

