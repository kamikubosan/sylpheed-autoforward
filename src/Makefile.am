
lib_LTLIBRARIES = autoforward.la

autoforward_la_SOURCES = autoforward.c
autoforward_la_LDFLAGS = -module -no-undefined -avoid-version

INCLUDES = \
	$(GTK_CFLAGS) \
	-I$(includedir) \
	$(SYLPHEED_CFLAGS)

autoforward_la_LIBADD = \
	$(GTK_LIBS) \
	$(SYLPHEED_LIBS)

install:
install-plugin: install-am
