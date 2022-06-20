# BASED ON: https://github.com/vector-of-bool/cmrc
# => Slightly simplified version, that generates headers with externs to
#  the byte array, rather than wrapping it in some string-accessed
#  resource 'filesystem'.

# This block is executed when generating an intermediate resource file, not when
# running in CMake configure mode
if(_RES_GENERATE_MODE)
    # Read in the digits
    file(READ "${INPUT_FILE}" bytes HEX)
    # Format each pair into a character literal. Heuristics seem to favor doing
    # the conversion in groups of five for fastest conversion
    string(REGEX REPLACE "(..)(..)(..)(..)(..)" "'\\\\x\\1','\\\\x\\2','\\\\x\\3','\\\\x\\4','\\\\x\\5'," chars "${bytes}")
    # Since we did this in groups, we have some leftovers to clean up
    string(LENGTH "${bytes}" n_bytes2)
    math(EXPR n_bytes "${n_bytes2} / 2")
    math(EXPR remainder "${n_bytes} % 5") # <-- '5' is the grouping count from above
    set(cleanup_re "$")
    set(cleanup_sub )
    while(remainder)
        set(cleanup_re "(..)${cleanup_re}")
        set(cleanup_sub "'\\\\x\\${remainder}',${cleanup_sub}")
        math(EXPR remainder "${remainder} - 1")
    endwhile()
    if(NOT cleanup_re STREQUAL "$")
        string(REGEX REPLACE "${cleanup_re}" "${cleanup_sub}" chars "${chars}")
    endif()
    string(CONFIGURE [[
#include <resources/@LIBNAME@.hpp>
namespace { const char file_array[] = { @chars@ 0 }; }
namespace resources { namespace @NAMESPACE@ {
extern const char* const @SYMBOL@_data = file_array;
extern const unsigned long long @SYMBOL@_size = sizeof(file_array);
}}]] code)
    file(WRITE "${OUTPUT_FILE}" "${code}")
    # Exit from the script. Nothing else needs to be processed
    return()
endif()

set(_version 2.0.0)

cmake_minimum_required(VERSION 3.3)
include(CMakeParseArguments)

if(COMMAND res_add_library)
    if(NOT DEFINED _RES_VERSION OR NOT (_version STREQUAL _RES_VERSION))
        message(WARNING "More than one EmbeddedResources version has been included in this project.")
    endif()
    # EmbeddedResources has already been included! Don't do anything
    return()
endif()

set(_RES_VERSION "${_version}" CACHE INTERNAL "EmbeddedResources version. Used for checking for conflicts")

set(_RES_SCRIPT "${CMAKE_CURRENT_LIST_FILE}" CACHE INTERNAL "Path to EmbeddedResources script")

function(_res_normalize_path var)
    set(path "${${var}}")
    file(TO_CMAKE_PATH "${path}" path)
    while(path MATCHES "//")
        string(REPLACE "//" "/" path "${path}")
    endwhile()
    string(REGEX REPLACE "/+$" "" path "${path}")
    set("${var}" "${path}" PARENT_SCOPE)
endfunction()

function(res_add_library name)
    set(args ALIAS NAMESPACE TYPE)
    cmake_parse_arguments(ARG "" "${args}" "" "${ARGN}")
    # Generate the identifier for the resource library's namespace
    set(ns_re "[a-zA-Z_][a-zA-Z0-9_]*")
    if(NOT DEFINED ARG_NAMESPACE)
        # Check that the library name is also a valid namespace
        if(NOT name MATCHES "${ns_re}")
            message(SEND_ERROR "Library name is not a valid namespace. Specify the NAMESPACE argument")
        endif()
        set(ARG_NAMESPACE "${name}")
    else()
        if(NOT ARG_NAMESPACE MATCHES "${ns_re}")
            message(SEND_ERROR "NAMESPACE for ${name} is not a valid C++ namespace identifier (${ARG_NAMESPACE})")
        endif()
    endif()
    set(libname "${name}")
    # Check that type is either "STATIC" or "OBJECT", or default to "STATIC" if
    # not set
    if(NOT DEFINED ARG_TYPE)
        set(ARG_TYPE STATIC)
    elseif(NOT "${ARG_TYPE}" MATCHES "^(STATIC|OBJECT)$")
        message(SEND_ERROR "${ARG_TYPE} is not a valid TYPE (STATIC and OBJECT are acceptable)")
        set(ARG_TYPE STATIC)
    endif()
    # Generate a library with the compiled in character arrays.
    string(CONFIGURE [=[
#pragma once

namespace resources {
namespace @ARG_NAMESPACE@ {
// These are the files which are available in this resource library
$<JOIN:$<TARGET_PROPERTY:@libname@,RES_EXTERN_DECLS>,
>
} // @ARG_NAMESPACE@
} // resources]=] hdr_content @ONLY)
    get_filename_component(libdir "${CMAKE_CURRENT_BINARY_DIR}/${name}" ABSOLUTE)
    get_filename_component(lib_inc_dir "${libdir}/include" ABSOLUTE)
    file(MAKE_DIRECTORY "${lib_inc_dir}/resources")
    get_filename_component(lib_inc_hdr "${lib_inc_dir}/resources/${name}.hpp" ABSOLUTE)
    string(REPLACE "\n        " "\n" hdr_content "${hdr_content}")
    file(GENERATE OUTPUT "${lib_inc_hdr}" CONTENT "${hdr_content}")
    # Generate the actual static library. Each source file is just a single file
    # with a character array compiled in containing the contents of the
    # corresponding resource file.
    add_library(${name} ${ARG_TYPE} ${libcpp})
    set_property(TARGET ${name} PROPERTY RES_LIBDIR "${libdir}")
    set_property(TARGET ${name} PROPERTY RES_NAMESPACE "${ARG_NAMESPACE}")
    target_include_directories(${name} PUBLIC "${lib_inc_dir}")
    set_property(TARGET ${name} PROPERTY RES_IS_RESOURCE_LIBRARY TRUE)
    if(ARG_ALIAS)
        add_library("${ARG_ALIAS}" ALIAS ${name})
    endif()
    target_sources(${name} PRIVATE ${lib_inc_hdr})
    res_add_resources(${name} ${ARG_UNPARSED_ARGUMENTS})
endfunction()

function(_res_register_dirs name dirpath)
    if(dirpath STREQUAL "")
        return()
    endif()
    # Skip this dir if we have already registered it
    get_target_property(registered "${name}" _RES_REGISTERED_DIRS)
    if(dirpath IN_LIST registered)
        return()
    endif()
    # Register the parent directory first
    get_filename_component(parent "${dirpath}" DIRECTORY)
    if(NOT parent STREQUAL "")
        _res_register_dirs("${name}" "${parent}")
    endif()
    # Now generate the registration
    set_property(TARGET "${name}" APPEND PROPERTY _RES_REGISTERED_DIRS "${dirpath}")
    _cm_encode_fpath(sym "${dirpath}")
    if(parent STREQUAL "")
        set(parent_sym root_directory)
    else()
        _cm_encode_fpath(parent_sym "${parent}")
    endif()
    get_filename_component(leaf "${dirpath}" NAME)
    set_property(
        TARGET "${name}"
        APPEND PROPERTY RES_MAKE_DIRS
        "static auto ${sym}_dir = ${parent_sym}_dir.directory.add_subdir(\"${leaf}\")\;"
        "root_index.emplace(\"${dirpath}\", &${sym}_dir.index_entry)\;"
        )
endfunction()

function(res_add_resources name)
    get_target_property(is_reslib ${name} RES_IS_RESOURCE_LIBRARY)
    if(NOT TARGET ${name} OR NOT is_reslib)
        message(SEND_ERROR "res_add_resources called on target '${name}' which is not an existing resource library")
        return()
    endif()

    set(options)
    set(args WHENCE PREFIX)
    set(list_args)
    cmake_parse_arguments(ARG "${options}" "${args}" "${list_args}" "${ARGN}")

    if(NOT ARG_WHENCE)
        set(ARG_WHENCE ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
    _res_normalize_path(ARG_WHENCE)
    get_filename_component(ARG_WHENCE "${ARG_WHENCE}" ABSOLUTE)

    # Generate the identifier for the resource library's namespace
    get_target_property(lib_ns "${name}" RES_NAMESPACE)

    get_target_property(libdir ${name} RES_LIBDIR)
    get_target_property(target_dir ${name} SOURCE_DIR)
    file(RELATIVE_PATH reldir "${target_dir}" "${CMAKE_CURRENT_SOURCE_DIR}")
    if(reldir MATCHES "^\\.\\.")
        message(SEND_ERROR "Cannot call res_add_resources in a parent directory from the resource library target")
        return()
    endif()

    foreach(input IN LISTS ARG_UNPARSED_ARGUMENTS)
        _res_normalize_path(input)
        get_filename_component(abs_in "${input}" ABSOLUTE)
        # Generate a filename based on the input filename that we can put in
        # the intermediate directory.
        file(RELATIVE_PATH relpath "${ARG_WHENCE}" "${abs_in}")
        if(relpath MATCHES "^\\.\\.")
            # For now we just error on files that exist outside of the soure dir.
            message(SEND_ERROR "Cannot add file '${input}': File must be in a subdirectory of ${ARG_WHENCE}")
            continue()
        endif()
        if(DEFINED ARG_PREFIX)
            _res_normalize_path(ARG_PREFIX)
        endif()
        if(ARG_PREFIX AND NOT ARG_PREFIX MATCHES "/$")
            set(ARG_PREFIX "${ARG_PREFIX}/")
        endif()
        get_filename_component(dirpath "${ARG_PREFIX}${relpath}" DIRECTORY)
        _res_register_dirs("${name}" "${dirpath}")
        get_filename_component(abs_out "${libdir}/intermediate/${relpath}.cpp" ABSOLUTE)
        # Generate a symbol name relpath the file's character array
        _cm_encode_fpath(sym "${relpath}")
        # Get the symbol name for the parent directory
        if(dirpath STREQUAL "")
            set(parent_sym root_directory)
        else()
            _cm_encode_fpath(parent_sym "${dirpath}")
        endif()
        # Generate the rule for the intermediate source file
        _res_generate_intermediate_cpp(${name} ${lib_ns} ${sym} "${abs_out}" "${abs_in}")
        target_sources(${name} PRIVATE "${abs_out}")
        target_sources(${name} PRIVATE ${abs_in})
        set_source_files_properties(${abs_in}
            PROPERTIES
            HEADER_FILE_ONLY TRUE)
        source_group("Resources" FILES ${abs_in})
        set_property(TARGET ${name} APPEND PROPERTY RES_EXTERN_DECLS
            "// Pointers to ${input}"
            "extern const char* const ${sym}_data\;"
            "extern const unsigned long long ${sym}_size\;"
            )
        get_filename_component(leaf "${relpath}" NAME)
        set_property(
            TARGET ${name}
            APPEND PROPERTY RES_MAKE_FILES
            "root_index.emplace("
            "    \"${ARG_PREFIX}${relpath}\","
            "    ${parent_sym}_dir.directory.add_file("
            "        \"${leaf}\","
            "        ${sym}_data,"
            "        ${sym}_size"
            "    )"
            ")\;"
            )
    endforeach()
endfunction()

function(_res_generate_intermediate_cpp lib_name lib_ns symbol outfile infile)
    add_custom_command(
        # This is the file we will generate
        OUTPUT "${outfile}"
        # These are the primary files that affect the output
        DEPENDS "${infile}" "${_RES_SCRIPT}"
        COMMAND
            "${CMAKE_COMMAND}"
                -D_RES_GENERATE_MODE=TRUE
                -DLIBNAME=${lib_name}
                -DNAMESPACE=${lib_ns}
                -DSYMBOL=${symbol}
                "-DINPUT_FILE=${infile}"
                "-DOUTPUT_FILE=${outfile}"
                -P "${_RES_SCRIPT}"
        COMMENT "Generating intermediate file for ${infile}"
    )
endfunction()

function(_cm_encode_fpath var fpath)
    string(MAKE_C_IDENTIFIER "${fpath}" ident)
    set(${var} ${ident} PARENT_SCOPE)
    # string(MAKE_C_IDENTIFIER "${fpath}" ident)
    # string(MD5 hash "${fpath}")
    # string(SUBSTRING "${hash}" 0 4 hash)
    # set(${var} res_${ident} PARENT_SCOPE)
endfunction()
