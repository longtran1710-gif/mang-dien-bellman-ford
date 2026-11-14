cmake_minimum_required(VERSION 3.16)

project(MangDienGUI VERSION 0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(MangDienGUI
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    bellmanford.cpp
    bellmanford.h
)

target_link_libraries(MangDienGUI PRIVATE Qt6::Widgets)

set_target_properties(MangDienGUI PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if(MSVC)
    target_compile_options(MangDienGUI PRIVATE /utf-8)
elseif(MINGW)
    target_compile_options(MangDienGUI PRIVATE
        -finput-charset=UTF-8
        -fexec-charset=UTF-8
    )
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(test_bellmanford tests/test_bellmanford.cpp bellmanford.cpp)
    target_include_directories(test_bellmanford PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    set_target_properties(test_bellmanford PROPERTIES AUTOMOC OFF AUTOUIC OFF)

    add_executable(test_gui
        tests/test_gui.cpp mainwindow.cpp mainwindow.h mainwindow.ui
        bellmanford.cpp bellmanford.h
    )
    target_include_directories(test_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(test_gui PRIVATE Qt6::Widgets)
    foreach(test_target test_bellmanford test_gui)
        if(MSVC)
            target_compile_options(${test_target} PRIVATE /utf-8)
        elseif(MINGW)
            target_compile_options(${test_target} PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
        endif()
    endforeach()
    add_test(NAME bellmanford COMMAND test_bellmanford)
    add_test(NAME gui COMMAND test_gui)
    set_tests_properties(gui PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()
