################################################################################
# CLI - A simple command line interface.
# Copyright (C) 2016-2021 Daniele Pallastrelli
#
# Boost Software License - Version 1.0 - August 17th, 2003
#
# Permission is hereby granted, free of charge, to any person or organization
# obtaining a copy of the software and accompanying documentation covered by
# this license (the "Software") to use, reproduce, display, distribute,
# execute, and transmit the Software, and to prepare derivative works of the
# Software, and to permit third-parties to whom the Software is furnished to
# do so, all subject to the following:
#
# The copyright notices in the Software and this entire statement, including
# the above license grant, this restriction and the following disclaimer,
# must be included in all copies of the Software, in whole or in part, and
# all derivative works of the Software, unless such copies or derivative
# works are solely in the form of machine-executable object code generated by
# a source language processor.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
# SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
# FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
################################################################################


# pluginmanager, filesession and simplelocalsession can be compiled w/o asio
set(SOURCES placeholdermenutest pluginmanager filesession simplelocalsession)

if (CLI_UseBoostAsio OR CLI_UseStandaloneAsio)
    if (CLI_UseBoostAsio)
        add_definitions( -DCLI_EXAMPLES_USE_BOOSTASIO_SCHEDULER )
        # boostasio_nonowner_iocontext can only be compiled with boost asio
        list(APPEND SOURCES boostasio_nonowner_iocontext)
    endif()
    if (CLI_UseStandaloneAsio)
        add_definitions( -DCLI_EXAMPLES_USE_STANDALONEASIO_SCHEDULER )
        # standaloneasio_nonowner_iocontext can only be compiled with standalone asio
        list(APPEND SOURCES standaloneasio_nonowner_iocontext)
    endif()
    # asyncsession, and complete can be compiled if asio is available (either standalone or boost)
    list(APPEND SOURCES complete)
    # asyncsession only compiles on POSIX platforms
    if(NOT WIN32)
        list(APPEND SOURCES asyncsession)
    else()
        message("example `asyncsession` not built as it's not supported on Windows")
    endif()
else()
    add_definitions( -DCLI_EXAMPLES_USE_LOOP_SCHEDULER )
    message("examples `nonowner_iocontext` and `complete` are not built because asio library is not available")
endif()

foreach(example ${SOURCES})
    add_executable(${example} ${example}.cpp)    
    target_link_libraries(${example} PRIVATE cli::cli)
endforeach(example)
