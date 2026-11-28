set(data_dir "${TEST_ROOT}/WordSprint/WordSprint")
file(MAKE_DIRECTORY "${data_dir}")
foreach(suffix IN ITEMS "" "-wal" "-shm" "-journal")
    file(WRITE "${data_dir}/wordsprint.db${suffix}" "test")
endforeach()
file(WRITE "${data_dir}/keep.txt" "keep")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "APPDATA=${TEST_ROOT}"
            "${CMAKE_COMMAND}" -P "${RESET_SCRIPT}"
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Database reset failed: ${result}")
endif()
foreach(suffix IN ITEMS "" "-wal" "-shm" "-journal")
    if(EXISTS "${data_dir}/wordsprint.db${suffix}")
        message(FATAL_ERROR "Database file was not removed: ${suffix}")
    endif()
endforeach()
if(NOT EXISTS "${data_dir}/keep.txt")
    message(FATAL_ERROR "Reset deleted an unrelated file.")
endif()
file(REMOVE "${data_dir}/keep.txt")

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env --unset=APPDATA
            "${CMAKE_COMMAND}" -P "${RESET_SCRIPT}"
    RESULT_VARIABLE result
    ERROR_QUIET
)
if(result EQUAL 0)
    message(FATAL_ERROR "Reset accepted a missing APPDATA.")
endif()
