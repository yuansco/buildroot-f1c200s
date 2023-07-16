################################################################################
#
# motion_angle
#
################################################################################

MOTION_ANGLE_VERSION:=master
MOTION_ANGLE_SITE=https://github.com/yuansco/Motion-Angle.git
MOTION_ANGLE_SITE_METHOD=git

define MOTION_ANGLE_BUILD_CMDS
	$(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define MOTION_ANGLE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/motion_angle $(TARGET_DIR)/usr/bin
	$(INSTALL) -D -m 0755 $(@D)/gsensor_show.sh $(TARGET_DIR)/root
endef

$(eval $(generic-package))


