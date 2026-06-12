# Generates an include file containing the current git commit hash so it
# can be embedded in rendered output (e.g. via the --annotate option).
#
# Expects SRC_DIR, TEMPLATE_FILE and DST_FILE to be set on the command line.

find_package(Git QUIET)

set(FLUXRT_GIT_COMMIT_HASH "unknown")

if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${SRC_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE GIT_RESULT
    )
    if(GIT_RESULT EQUAL 0)
        set(FLUXRT_GIT_COMMIT_HASH "${GIT_HASH}")

        execute_process(
            COMMAND ${GIT_EXECUTABLE} diff --quiet HEAD
            WORKING_DIRECTORY ${SRC_DIR}
            RESULT_VARIABLE GIT_DIRTY_RESULT
        )
        if(NOT GIT_DIRTY_RESULT EQUAL 0)
            set(FLUXRT_GIT_COMMIT_HASH "${FLUXRT_GIT_COMMIT_HASH}-dirty")
        endif()
    endif()
endif()

configure_file(${TEMPLATE_FILE} ${DST_FILE} @ONLY)
