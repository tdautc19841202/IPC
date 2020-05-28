.PHONY: release image
include configs/current.configs
-include $(SOURCE_MK)

image: symbol_link
	$(MAKE) headfile_link
	$(MAKE) error_message
	$(MAKE) release
	$(MAKE) image_install

image-nocheck: symbol_link
	$(MAKE) headfile_link
	$(MAKE) release
	$(MAKE) image_install
	$(MAKE) warning_message

image-fast:
	$(MAKE) headfile_link
	$(MAKE) error_message
	$(MAKE) image_install

image-fast-nocheck:
	$(MAKE) headfile_link
	$(MAKE) image_install
	$(MAKE) warning_message

image-ota:
	$(MAKE) -C image ota_images

clean:
	$(MAKE) $(SOURCE_CLEAN)
	$(MAKE) image_clean
	$(MAKE) symbol_link_clean
	$(MAKE) headfile_link_clean

image_install: image_clean
	$(MAKE) -C image all

image_clean:
	$(MAKE) -C image clean

release:
ifneq ($(SOURCE_RELEASE), )
	$(MAKE) sdk
endif

build_verify: $(SOURCE_RELEASE_DEPEND) clean image

symbol_link: kbuild/$(KERNEL_VERSION)/.config

symbol_link_clean:
	$(MAKE) -f kbuild/$(KERNEL_VERSION)/kbuild.mk symbol_link_clean

kbuild/$(KERNEL_VERSION)/.config:
	$(MAKE) -f kbuild/$(KERNEL_VERSION)/kbuild.mk symbol_link

headfile_link:
	ln -vsf $(PROJ_ROOT)/release/include/isp/$(CHIP_ALIAS)/*.h $(PROJ_ROOT)/release/include

headfile_link_clean:
	rm -vf $(PROJ_ROOT)/release/include/mi_isp*.h

warning_message:
	@if [ -d ".git" ]; then \
		cd  release/include && \
		git status -s . | \
		awk -F' ' '{a = a "\033[7;34;42m""release/include/"$$2"\033[0m\n"} END {if(a){ \
				print "\033[5;41;33mWARNING!!!! You CAN NOT MODIFY HEAD FILES ,NEED MI OWNER TO REVIEW THE FOLLOWING INCONSISTENT HEAD FILES!!!\033[0m"; \
				print a;}}'; \
	fi;

error_message:warning_message
	@if [ -d ".git" ]; then \
		cd  release/include && \
		git status -s . | \
		awk -F' ' '{a = a "\033[7;34;42m""release/include/"$$2"\033[0m\n"} END {if(a){ \
				print "\033[5;41;33mIF YOU WANT DEBUG ONLY, pelase using command :[[[ make image-nocheck ]]] INSTEADED!!!\033[0m"; \
				exit 127;}}'; \
	fi;
