set(srcs
	"ble.c"
)

idf_component_register(SRCS "ble.c"
                    INCLUDE_DIRS ".")

set_target_properties(${COMPONENT_LIB} PROPERTIES COMPILE_FLAGS -Wno-unused-const-variable)