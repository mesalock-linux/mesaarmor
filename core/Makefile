prefix ?= $(DESTDIR)/usr
sysconfdir ?= $(DESTDIR)/etc
PYTHON_VER ?= 2.4

all:
	$(MAKE) -C lib
	$(MAKE) -C converter
	$(MAKE) -C utils
	$(MAKE) -C restorecon
	$(MAKE) -C config

install: all
	$(MAKE) -C lib install prefix=$(prefix) sysconfdir=$(sysconfdir)
	$(MAKE) -C converter install  prefix=$(prefix) sysconfdir=$(sysconfdir)
	$(MAKE) -C utils install   prefix=$(prefix) sysconfdir=$(sysconfdir)
	$(MAKE) -C restorecon install  prefix=$(prefix) sysconfdir=$(sysconfdir)
	$(MAKE) -C config install  prefix=$(prefix) sysconfdir=$(sysconfdir)

uninstall:
	$(MAKE) -C lib uninstall  prefix=$(prefix) sysconfdir=$(sysconfdir)
	$(MAKE) -C converter uninstall  prefix=$(prefix) sysconfdir=$(sysconfdir)
	$(MAKE) -C utils uninstall   prefix=$(prefix) sysconfdir=$(sysconfdir)

cross-install: all
	mkdir -p $(DESTDIR)
	cp converter/seedit-converter $(DESTDIR)
	cp -r config/macros $(DESTDIR)
	cp -r config/base_policy $(DESTDIR)
	cp utils/seedit-cross.sh $(DESTDIR)
	cp utils/audit2spdl.py $(DESTDIR)/audit2spdl
	cp utils/seedit-load.py $(DESTDIR)/seedit-load
	cp -r lib/python/seedit $(DESTDIR)
	cp config/seedit.conf   $(DESTDIR)
clean:
	$(MAKE) -C lib clean
	$(MAKE) -C converter clean
	$(MAKE) -C utils clean
	$(MAKE) -C restorecon clean
test: all
	cd testdata; make 	
