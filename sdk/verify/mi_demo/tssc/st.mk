INC += $(wildcard $(foreach m,$(LIBS_PATH),$(m)/*))
LIBS += $(foreach m,$(ST_DEP),-l$(m))