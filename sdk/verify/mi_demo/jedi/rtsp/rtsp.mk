INC  += $(DB_BUILD_TOP)/../common/live555/UsageEnvironment/include
INC  += $(DB_BUILD_TOP)/../common/live555/groupsock/include
INC  += $(DB_BUILD_TOP)/../common/live555/liveMedia/include
INC  += $(DB_BUILD_TOP)/../common/live555/BasicUsageEnvironment/include
INC  += $(DB_BUILD_TOP)/../common/live555/mediaServer/include
LIBS += -L./lib

ST_DEP := common vpe venc vif rgn onvif ao live555 tem

LIBS += -lmi_sys -lmi_common -lmi_vpe -lmi_isp -lmi_sensor -lmi_venc -lmi_vif -lmi_rgn -lmi_divp -lmi_iqserver -lmi_vdf \
		-lmi_shadow -lOD_LINUX -lMD_LINUX -lVG_LINUX -lmi_ive -lmi_ao -lcus3a -lispalgo
