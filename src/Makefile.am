
lib_LTLIBRARIES = autoforward.la

autoforward_la_SOURCES = autoforward.c
autoforward_la_LDFLAGS = -module -no-undefined -avoid-version

INCLUDES = \
	$(GTK_CFLAGS) \
	-I$(includedir) \
	-I$(sylpheed_build_dir) \
	-I$(sylpheed_build_dir)/libsylph \
	-I$(sylpheed_build_dir)/src

autoforward_la_LIBADD = \
	$(GTK_LIBS) \
	$(sylpheed_build_dir)/src/libsylpheed-plugin-0.la \
	$(sylpheed_build_dir)/libsylph/libsylph-0.la

install:
install-plugin: install-am
