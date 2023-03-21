################################################################################
#
# ssd1306_linux
#
################################################################################

SSD1306_LINUX_VERSION:=master
SSD1306_LINUX_SITE:=https://github.com/armlabs/ssd1306_linux/archive/master
SSD1306_LINUX_METHOD:=git


define SSD1306_LINUX_BUILD_CMDS
    $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D)
endef

define SSD1306_LINUX_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/ssd1306_bin $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
