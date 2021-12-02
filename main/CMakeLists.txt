idf_component_register(SRCS "main.cpp" "time_utils.cpp"
                    INCLUDE_DIRS ""
                    REQUIRES ulp speech-recognition vibration-motor display ble nvs_flash)

set(ulp_app_name ulp_mic)

set(ulp_s_sources "../ulp/mic.S")

set(ulp_exp_dep_srcs "main.cpp")

ulp_embed_binary(${ulp_app_name} ${ulp_s_sources} ${ulp_exp_dep_srcs})