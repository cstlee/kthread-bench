CC = g++
CFLAGS = -I /usr/include/x86_64-linux-gnu
LFLAGS = -lrt

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
