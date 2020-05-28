INC  += $(DB_BUILD_TOP)/../common/live555/UsageEnvironment/include
INC  += $(DB_BUILD_TOP)/../common/live555/groupsock/include
INC  += $(DB_BUILD_TOP)/../common/live555/liveMedia/include
INC  += $(DB_BUILD_TOP)/../common/live555/BasicUsageEnvironment/include
INC  += $(DB_BUILD_TOP)/../common/live555/mediaServer/include
INC  += $(DB_BUILD_TOP)/internal/base/include/
ST_DEP := base common vpe venc vif rgn onvif live555 tem iniparser

LIBS += -lmi_vif -lmi_vpe -lmi_venc -lmi_rgn -lmi_divp -lmi_isp -lmi_iqserver -lcus3a -lispalgo
