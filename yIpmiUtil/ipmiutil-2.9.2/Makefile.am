
SUBDIRS = @SUBDIR_S@
PKGDIR = @PKG_DIR@
SOURCEDIR = $(PKGDIR)/SOURCES
BUILDDIR = $(PKGDIR)/BUILD
RSPEC = doc/$(PACKAGE).spec
PNAME  = $(PACKAGE)-$(VERSION)
TMPDIR = /var/tmp/$(PNAME)
BDIR = $(TMPDIR)-1-root
RPMB = rpmbuild
# $(RPMB) -bb --buildroot $(BDIR) $(RSPEC)

rpm:	$(RSPEC)
	make tarball
	$(RPMB) -bb  $(RSPEC)
	$(RPMB) -bs  $(RSPEC)

tarball:
	rm -f $(SOURCEDIR)/$(PNAME).tar.gz 
	rm -rf $(TMPDIR)
	mkdir -p $(TMPDIR)
	mkdir -p $(SOURCEDIR)
	find . -print | cpio -pdumv $(TMPDIR)
	chown -R root $(TMPDIR)
	chgrp -R root $(TMPDIR)
	rm -f $(TMPDIR)/lib/libipmiapi*.a* 
	cd $(TMPDIR)/test; make clean; cd ..
	cd $(TMPDIR)/hpiutil; make clean; cd ..
	cd $(TMPDIR); make distclean
	cd $(TMPDIR); rm -rf autom4te.cache
	cd $(TMPDIR); rm -f  util/events
	cd $(TMPDIR); sed -i 's/\r//' ChangeLog AUTHORS COPYING NEWS README TODO doc/UserGuide
	cd $(TMPDIR); chmod 644 doc/UserGuide
	cd $(TMPDIR); cd ..; tar -czvf $(SOURCEDIR)/$(PNAME).tar.gz $(PNAME)
	rm -rf $(TMPDIR)

tarsol:        util/ipmiutil
	rm -f /tmp/$(PNAME)-solaris.tar
	rm -rf $(TMPDIR)
	mkdir -p $(TMPDIR)
	cd util; cp ipmiutil idiscover ievents ipmi_port $(TMPDIR)
	cd doc; cp UserGuide *.8 *.mib  $(TMPDIR)
	cd scripts; cp checksel ipmiutil_??? *.sh $(TMPDIR)
	cd $(TMPDIR); mv install-solaris.sh install.sh
	cd $(TMPDIR); tar cvf /tmp/$(PNAME)-solaris.tar .
	rm -rf $(TMPDIR)

tarbsd:        util/ipmiutil
	rm -f /tmp/$(PNAME)-bsd.tar
	rm -rf $(TMPDIR)
	mkdir -p $(TMPDIR)
	cd util; cp ipmiutil idiscover ievents ipmi_port $(TMPDIR)
	cd doc; cp UserGuide *.8 *.mib  $(TMPDIR)
	cd scripts; cp checksel ipmiutil_??? *.sh $(TMPDIR)
	cd $(TMPDIR); mv install-solaris.sh install.sh
	cd $(TMPDIR); tar -cvf /tmp/$(PNAME)-bsd.tar .
	rm -rf $(TMPDIR)

clean-generic:
	rm -f *.log *.tmp debug*.list
