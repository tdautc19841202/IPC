LIBS += -L$(DB_BUILD_TOP)/$(MODULE)/lib/
LIBS += -L$(DB_BUILD_TOP)/$(MODULE)/thirdlib
LIBS += -lISMgrProvider -lcares -lcrypto -lcurl -lsqlite3 -lssl -lxml2 -lz
MODULE_REL_FILES += $(DB_BUILD_TOP)/$(MODULE)/lib
MODULE_REL_FILES += $(DB_BUILD_TOP)/$(MODULE)/thirdlib
MODULE_REL_FILES += $(DB_BUILD_TOP)/$(MODULE)/dnslib
MODULE_REL_FILES += $(DB_BUILD_TOP)/$(MODULE)/OTA
MODULE_REL_FILES += $(DB_BUILD_TOP)/$(MODULE)/run.sh
