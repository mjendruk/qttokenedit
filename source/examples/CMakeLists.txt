
# Check if examples are enabled
if(NOT OPTION_BUILD_EXAMPLES)
    return()
endif()

# Example applications
add_subdirectory(customer_input)
add_subdirectory(sorted_table)
add_subdirectory(tokenedit_playground)
