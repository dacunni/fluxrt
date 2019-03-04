
INCLUDE_DIRECTORIES(..)

option(RUN_INSPECTION_TESTS "Run tests that generate output files for inspection" OFF)

ADD_EXECUTABLE (test_image_write test_image_write.cpp)
TARGET_LINK_LIBRARIES (test_image_write micromath)

if(RUN_INSPECTION_TESTS)
    ADD_CUSTOM_COMMAND(TARGET test_image_write POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/image)
    ADD_CUSTOM_COMMAND(TARGET test_image_write POST_BUILD
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/image
                       COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_image_write)
endif(RUN_INSPECTION_TESTS)

ADD_EXECUTABLE (test_simple_intersection test_simple_intersection.cpp)
TARGET_LINK_LIBRARIES (test_simple_intersection micromath)

if(RUN_INSPECTION_TESTS)
    ADD_CUSTOM_COMMAND(TARGET test_simple_intersection POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/simple_intersection)
    ADD_CUSTOM_COMMAND(TARGET test_simple_intersection POST_BUILD
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/simple_intersection
                       COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_simple_intersection)
endif(RUN_INSPECTION_TESTS)

ADD_EXECUTABLE (test_fresnel test_fresnel.cpp)
TARGET_LINK_LIBRARIES (test_fresnel micromath)

if(RUN_INSPECTION_TESTS)
    ADD_CUSTOM_COMMAND(TARGET test_fresnel POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fresnel
                       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fresnel/images)
    ADD_CUSTOM_COMMAND(TARGET test_fresnel POST_BUILD
                       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/fresnel
                       COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_fresnel
                       COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/plot_xy.py images/fresnel_schlick_v_angle.png fresnel_schlick_*txt
                       COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/plot_xy.py images/snell_angle_vaccuum_to_all.png snell_angle_ni_1.00_nt_*txt
                       COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/plot_xy.py images/snell_angle_all_to_vaccuum.png snell_angle_ni_*_nt_1.00.txt
                       COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/plot_xy.py images/fresnel_dialectric_ni_1.00_nt_1.50.png fresnel_dialectric_*_ni_1.00_nt_1.50.txt
                       COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/plot_xy.py images/fresnel_dialectric_ni_1.50_nt_1.00.png fresnel_dialectric_*_ni_1.50_nt_1.00.txt
                       COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/plot_xy.py images/beers_law.png beers_law_*.txt)
endif(RUN_INSPECTION_TESTS)

