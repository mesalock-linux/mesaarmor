# $Id: Makefile,v 1.11 2006/04/28 16:48:28 ynakam Exp $

#! SELinux Policy Editor, a simple editor for SELinux policies
#! Copyright (C) 2006 Yuichi Nakamura
sysconfdir =/etc
CONVERTER=/usr/bin/seedit-converter
LOADPOLICY=/usr/sbin/load_policy
CHECKPOLICY=/usr/bin/checkpolicy
FIXFILES=/sbin/fixfiles
RESTORECON=/usr/sbin/seedit-restorecon
SELINUXTYPE=seedit
POLICYROOT=$(sysconfdir)/selinux/$(SELINUXTYPE)
POLICYDIR=$(POLICYROOT)/policy
CONTEXTSDIR=$(POLICYROOT)/contexts
CONFDIR=./simplified_policy
BASEPOLICYDIR=./base_policy
MACRODIR=./macros
OUTDIR =./sepolicy
INSTALL_PATH = $(POLICYROOT)
SELINUXCONF=$(sysconfdir)/selinux/config

M4=m4
DEFAULT_POLICYVER=18
POLICYVER := $(shell $(CHECKPOLICY)  -V |cut -f 1 -d ' ')
ifeq ($(POLICYVER),)
POLICYVER=$(DEFAULT_POLICYVER)
endif

FILE_CONTEXTS=  $(CONTEXTSDIR)/files/file_contexts.m4
OLD_FILE_CONTEXTS=  $(CONTEXTSDIR)/files/file_contexts.m4.old


policy:
	mkdir -p $(OUTDIR);
	m4 -s $(CONFDIR)/*.sp >$(CONFDIR)/all.sp;
	$(CONVERTER) -i $(CONFDIR)/all.sp -o $(OUTDIR) -b $(BASEPOLICYDIR) -I $(CONFDIR)/include ;
	$(M4)  -Imacros -s $(MACRODIR)/*.te $(OUTDIR)/generated.conf > $(OUTDIR)/policy.conf;
	$(M4)  -Imacros -s $(MACRODIR)/mcs_macros.te $(OUTDIR)/file_contexts.m4 > $(OUTDIR)/file_contexts;
	$(M4)  -Imacros -s $(MACRODIR)/mcs_macros.te $(OUTDIR)/userhelper_context.m4 > $(OUTDIR)/userhelper_context.tmp;
	grep system_u $(OUTDIR)/userhelper_context.tmp >  $(OUTDIR)/userhelper_context
	rm $(OUTDIR)/userhelper_context.tmp
	$(CHECKPOLICY) -o $(OUTDIR)/policy.$(POLICYVER) -c $(POLICYVER) $(OUTDIR)/policy.conf

install: policy
	cp $(OUTDIR)/policy.$(POLICYVER) $(POLICYDIR)
	cp $(OUTDIR)/unconfined_domains $(POLICYDIR)
	cp $(OUTDIR)/file_contexts $(CONTEXTSDIR)/files/
	cp $(OUTDIR)/file_contexts.m4 $(CONTEXTSDIR)/files/
	cp $(OUTDIR)/customizable_types $(CONTEXTSDIR)
	cp $(OUTDIR)/userhelper_context $(CONTEXTSDIR)
	cp -r $(BASEPOLICYDIR)/contexts/*  $(CONTEXTSDIR) 
	echo "" > $(CONTEXTSDIR)/files/file_contexts.homedirs

relabel: install
	$(LOADPOLICY) $(POLICYDIR)/policy.$(POLICYVER)
	$(FIXFILES) -F restore
	cp $(FILE_CONTEXTS) $(OLD_FILE_CONTEXTS)

diffrelabel: install
	if [ -e fcdiff.tmp ] ; then \
		exec 3< fcdiff.tmp;\
		while read FLs 0<&3; do \
		for FL in $$FLs; do \
		  	if [ -e $$FL ] ; then \
		    		$(RESTORECON) $$FL -R -vv;\
			fi;\
		done; \
		done; \
		exec 3<&-;\
	fi;
	$(LOADPOLICY) $(POLICYDIR)/policy.$(POLICYVER)
	diff $(OLD_FILE_CONTEXTS) $(FILE_CONTEXTS) -r |cat >  fcdiff.m4.tmp
	$(M4)  -Imacros -s $(MACRODIR)/mcs_macros.te fcdiff.m4.tmp > fcdiff.pre.tmp
	cat fcdiff.pre.tmp |grep -e "^[<>]"|sed -e 's/^[><][ \t]*//'|sed -e 's/[ \t]\+.*//'|sed -e 's/(.*//'|sort|sed s/"\[\^\/\]"//g > fcdiff.tmp
	exec 3< fcdiff.tmp;\
	while read FLs 0<&3; do \
	for FL in $$FLs; do \
	  if [ -e $$FL ] ; then \
	    $(RESTORECON) $$FL -R -vv;\
	  fi;\
	done;\
	done; \
	exec 3<&-;\
	cp $(FILE_CONTEXTS) $(OLD_FILE_CONTEXTS)
	rm fcdiff.tmp
	rm fcdiff.m4.tmp
	rm fcdiff.pre.tmp



