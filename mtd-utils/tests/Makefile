
SUBDIRS = checkfs fs-tests jittertest

all clean tests: $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)
