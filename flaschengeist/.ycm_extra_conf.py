"""
YouCompleteMe extra configuration for Platformio based
projects.

Based on the `.ycm_extra_conf.py` by @ladislas in his Bare-Arduino-Project.

Anthony Ford <github.com/ajford>

"""
import os
import ycm_core
import logging

# Logger for additional logging.
# To enable debug logging, add `let g:ycm_server_log_level = 'debug'` to
# your .vimrc file.
logger = logging.getLogger('ycm-extra-conf')

# Platformio Autogen libs.
## Platformio automatically copies over the libs you use after your first run.
## Be warned that you will not receive autocompletion on libraries until after
## your first `platformio run`.
PlatformioAutogen = ".pioenvs/"

# All Platformio Arduino Libs
## This will link directly to the Platformio Libs for Arduino.
## Be warned that this can polute your namespace (in #include)
## and slightly increase startup time (while crawling the lib
## dir for header files). This will however allow you to
## complete for header files you haven't included yet.
PlatformioArduinoLibs ="~/.platformio/packages/framework-arduinoespressif8266/libraries/"

# Platformio Arduino Core
## This links to the Platformio Arduino Cores. This provides
## the core libs, such as Arduino.h and HardwareSerial.h
PlatformioArduinoCore ="~/.platformio/packages/framework-arduinoespressif8266/cores/esp8266/"

# Platformio Arduino Std Libs
## Arduino Std libs from .platformio packages. Provides stdlib.h and such.
PlatformioArduinoSTD = "~/.platformio/packages/toolchain-xtensa/xtensa-lx106-elf/include/"

NeoPixelLibStd = "~/.platformio/lib/NeoPixelBus_ID547/src"
WebSocketLibStd= "~/.platformio/lib/WebSockets_ID549/src"
WEMOS_D1_mini_pins = "~/.platformio/packages/framework-arduinoespressif8266/variants/d1_mini/"

# This is the list of all directories to search for header files.
# Dirs in this list can be paths relative to this file, absolute
# paths, or paths relative to the user (using ~/path/to/file).
libDirs = [
           "lib"
           ,PlatformioAutogen
           ,PlatformioArduinoCore
           ,PlatformioArduinoLibs
           ,PlatformioArduinoSTD
		   ,NeoPixelLibStd
		   ,WebSocketLibStd
		   ,WEMOS_D1_mini_pins
           ]

flags = [
  # General flags
  '-Wall',
  '-Wextra',
  '-Werror',
  '-Wc++98-compat',
  '-std=c++11',
  '-ansi',
  '-x',
  'c++',
  # include files (to get code completion using clang)
  '-I', '.',
  '-I', './src',
  '-I', './lib/TickerScheduler',
  '-I', './.piolibdeps/WebSockets_ID549/src',
  '-I', './.piolibdeps/NeoPixelBus_ID547/src',
  '-I', './.piolibdeps/ESPAsyncTCP_ID305/src',
  '-I', './.piolibdeps/ArduinoJson_ID64',
  # Hier habe ich einfach die Ausgaben von Platformio eingetragen
  '-DF_CPU=80000000L',
  '-DPLATFORMIO=021101',
  '-D__ets__',
  '-DICACHE_FLASH',
  '-DESP8266',
  '-DARDUINO_ARCH_ESP8266',
  '-DESP8266_WEMOS_D1MINI',
  '-DARDUINO=20300',
]


compilation_database_folder = ''

if os.path.exists( compilation_database_folder ):
    database = ycm_core.CompilationDatabase( compilation_database_folder )
else:
    database = None

SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', '.ino', '.m', '.mm' ]

def DirectoryOfThisScript():
    return os.path.dirname( os.path.abspath( __file__ ) )

def MakeRelativePathsInFlagsAbsolute( flags, working_directory ):
    if not working_directory:
        return list( flags )

    new_flags = []
    make_next_absolute = False
    path_flags = [ '-isystem', '-I', '-iquote', '--sysroot=' ]

    for libDir in libDirs:

        # dir is relative to $HOME
        if libDir.startswith('~'):
            libDir = os.path.expanduser(libDir)

        # dir is relative to `working_directory`
        if not libDir.startswith('/'):
            libDir = os.path.join(working_directory,libDir)

        # Else, assume dir is absolute

        for path, dirs, files in os.walk(libDir):
            # Add to flags if dir contains a header file and is not
            # one of the metadata dirs (examples and extras).
            if any(IsHeaderFile(x) for x in files) and\
              path.find("examples") is -1 and path.find("extras") is -1:
                logger.debug("Directory contains header files - %s"%path)
                flags.append('-I'+path)


    for flag in flags:
        new_flag = flag

        if make_next_absolute:
            make_next_absolute = False
            if not flag.startswith( '/' ):
                new_flag = os.path.join( working_directory, flag )

        for path_flag in path_flags:
            if flag == path_flag:
                make_next_absolute = True
                break

            if flag.startswith( path_flag ):
                path = flag[ len( path_flag ): ]
                new_flag = path_flag + os.path.join( working_directory, path )
                break

        if new_flag:
            new_flags.append( new_flag )
    return new_flags


def IsHeaderFile( filename ):
    extension = os.path.splitext( filename )[ 1 ]
    return extension in [ '.h', '.hxx', '.hpp', '.hh' ]


def GetCompilationInfoForFile( filename ):
    # The compilation_commands.json file generated by CMake does not have entries
    # for header files. So we do our best by asking the db for flags for a
    # corresponding source file, if any. If one exists, the flags for that file
    # should be good enough.
    if IsHeaderFile( filename ):
        basename = os.path.splitext( filename )[ 0 ]
        for extension in SOURCE_EXTENSIONS:
            replacement_file = basename + extension
            if os.path.exists( replacement_file ):
                compilation_info = database.GetCompilationInfoForFile(
                    replacement_file )
                if compilation_info.compiler_flags_:
                    return compilation_info
        return None
    return database.GetCompilationInfoForFile( filename )


def FlagsForFile( filename, **kwargs ):
    if database:
        # Bear in mind that compilation_info.compiler_flags_ does NOT return a
        # python list, but a "list-like" StringVec object
        compilation_info = GetCompilationInfoForFile( filename )
        if not compilation_info:
            return None

        final_flags = MakeRelativePathsInFlagsAbsolute(
            compilation_info.compiler_flags_,
            compilation_info.compiler_working_dir_ )

    else:
        relative_to = DirectoryOfThisScript()
        final_flags = MakeRelativePathsInFlagsAbsolute( flags, relative_to )

    return {
        'flags': final_flags,
        'do_cache': True
    }
