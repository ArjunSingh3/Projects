

add_executable(lab8_m3.elf main_m3.c missile.c gameControl.c plane.c)
target_link_libraries(lab8_m3.elf ${330_LIBS} touchscreen interrupts intervalTimer buttons_switches)
set_target_properties(lab8_m3.elf PROPERTIES LINKER_LANGUAGE CXX)