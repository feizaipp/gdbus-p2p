AM_CPPFLAGS = \
	$(GLIB_CFLAGS) \
	$(GIO_CFLAGS) \
	-DPACKAGE_LIBEXEC_DIR=\""$(libexecdir)"\"

LIBS = \
	$(GLIB_LIBS) \
	$(GIO_LIBS) \
	$(NULL)

dbus_built_sources = gdbusp2p-generated.h gdbusp2p-generated.c

$(dbus_built_sources) : $(top_srcdir)/data/org.freedesktop.Gdbusp2p.xml
	gdbus-codegen --interface-prefix org.freedesktop.Gdbusp2p --c-namespace Gdbusp2p --generate-c-code=gdbusp2p-generated $<

BUILT_SOURCES =										\
	$(dbus_built_sources)								\
	$(NULL)

gdbusp2pprivdir = $(libexecdir)/gdbusp2p
gdbusp2ppriv_PROGRAMS = gdbusp2pd
gdbusp2pd_SOURCES = server.c $(BUILT_SOURCES)

bin_PROGRAMS = client
client_SOURCES = client.c $(BUILT_SOURCES)

CLEANFILES = $(BUILT_SOURCES)


