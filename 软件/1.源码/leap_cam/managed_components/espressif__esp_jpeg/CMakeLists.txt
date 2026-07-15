set(sources "jpeg_decoder.c")
set(includes "include")

# Compile only when cannot use ROM code
if(NOT CONFIG_JD_USE_ROM)
    list(APPEND sources "tjpgd/tjpgd.c")
    list(APPEND includes "tjpgd")
endif()

if(CONFIG_JD_DEFAULT_HUFFMAN)
    list(APPEND sources "jpeg_default_huffman_table.c")
endif()

idf_component_register(SRCS ${sources} INCLUDE_DIRS ${includes})
