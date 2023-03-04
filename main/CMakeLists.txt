

idf_component_register(SRCS
    "Adc.cpp"
    # "AM2320.cpp"
    "Button.cpp"
    "Component.cpp"
    "Display.cpp"
    "DisplayMode.cpp"
    #"I2C.cpp"
    "ISRServiceable.cpp"
    "Led.cpp"
    "main.cpp"
    "MainDisplayMode.cpp"
    #"LiquidCrystal_I2C.cpp"
    #"Print.cpp"
    "Ssd1351.cpp"
    "Thermistor.cpp"
    INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)
