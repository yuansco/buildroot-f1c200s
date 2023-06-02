################################################################################
#
# motion_angle
#
################################################################################

MOTION_ANGLE_VERSION = 1.0
MOTION_ANGLE_SITE = ./package/motion_angle
MOTION_ANGLE_SITE_METHOD = local

define MOTION_ANGLE_BUILD_CMDS
    $(MAKE) -C $(@D) $(TARGET_CONFIGURE_OPTS)
endef

define MOTION_ANGLE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/motion_angle $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
