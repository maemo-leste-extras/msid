
#
# handwritten makefiles are cool
#

COMPILER = g++
PROGRAM = msid

MSID_SEARCH_IFACE = ../search_iface.h ../curling.h ../curling.c

TP2B_SEARCH_PLUGIN = $(MSID_SEARCH_INTERFACE) search_plugin_tp2b.h search_plugin_tp2b.cpp
SUPER_SEARCH_PLUGIN = $(MSID_SEARCH_INTERFACE) search_plugin_super.h search_plugin_super.cpp

DEPS = `pkg-config --libs --cflags glib-2.0 gthread-2.0 gtk+-2.0` -lsidplay -lcurl

SEARCH_PLUGIN_DIR = $(DESTDIR)/opt/msid/usr/lib/$(PROGRAM)/search

all :
	$(COMPILER) -fPIC -o libmsid_super.so -shared $(SUPER_SEARCH_PLUGIN) $(DEPS)

install :
	install -d $(SEARCH_PLUGIN_DIR)
	install -m 644 *.so  $(SEARCH_PLUGIN_DIR)

clean :
	rm -f lib*so core* *~
