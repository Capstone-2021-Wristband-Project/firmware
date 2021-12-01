idf_component_register(
    SRCS speech_recognition.cc audio_provider.cc feature_provider.cc
         model.cc recognize_commands.cc command_responder.cc
         micro_features_generator.cc micro_model_settings.cc ringbuf.c
    REQUIRES tflite
    INCLUDE_DIRS ".")
