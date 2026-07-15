idf_component_register(SRCS "tjpgd_test.c" "test_tjpgd_main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES "unity"
                       WHOLE_ARCHIVE
                       EMBED_FILES "logo.jpg" "usb_camera.jpg" "usb_camera_2.jpg")
