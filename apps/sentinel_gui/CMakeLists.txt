add_executable(sentinel_gui main.cpp)

target_link_libraries(sentinel_gui
    PRIVATE
        sentinel_gui_lib
) 