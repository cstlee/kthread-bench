CC = g++
CFLAGS = -std=c++11 -Wall -Werror \
			-I/usr/include/x86_64-linux-gnu \
			-Ilib/PerfUtils/include \
			-Ilib/docopt.cpp
LFLAGS = -lrt \
			-Llib/PerfUtils/lib -lPerfUtils \
			-Llib/docopt.cpp -l:libdocopt.a

SRCDIR = src
OBJDIR = obj
BINDIR = bin
SRCEXT = cc
OBJEXT = o
DEPEXT = d

src = $(wildcard $(SRCDIR)/*.$(SRCEXT))
obj = $(src:$(SRCDIR)/%.$(SRCEXT)=$(OBJDIR)/%.$(OBJEXT))
dep = $(obj:.$(OBJEXT)=.$(DEPEXT))

.SUFFIXES:

all: $(BINDIR)/dispatch $(BINDIR)/worker

$(BINDIR)/dispatch: $(OBJDIR)/dispatch.$(OBJEXT)
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $^ $(LFLAGS)

$(BINDIR)/worker: $(OBJDIR)/worker.$(OBJEXT)
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $(CFLAGS) $^ $(LFLAGS)

-include $(dep)

$(OBJDIR)/%.$(DEPEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.$(DEPEXT)=.$(OBJEXT)) >$@

$(OBJDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(OBJDIR)
	$(CC) -c -o $@ $(CFLAGS) $<

.PHONY: clean
clean:
	rm -f $(obj) $(dep) $(BINDIR)/*
