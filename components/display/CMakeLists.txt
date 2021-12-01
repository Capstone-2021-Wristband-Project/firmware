file(GLOB U8g2_srcs
    "U8g2/src/**.cpp"
    "U8g2/src/clib/**.c"
)

set(srcs
    "${U8g2_srcs}"
    "display.cpp"
)

idf_component_register(
    SRCS "${srcs}"
    INCLUDE_DIRS "." "U8g2/src"
    REQUIRES arduino)

set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS "-Wno-unused-const-variable -Wno-maybe-uninitialized")
