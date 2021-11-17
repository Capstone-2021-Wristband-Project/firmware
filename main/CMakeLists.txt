idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS ""
                    REQUIRES ulp)

set(ulp_app_name ulp_mic)

set(ulp_s_sources "../ulp/mic.S")

set(ulp_exp_dep_srcs "main.cpp")

ulp_embed_binary(${ulp_app_name} ${ulp_s_sources} ${ulp_exp_dep_srcs})
