set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# configure moc (works similarly for uic and rcc)

set(path_moc /usr/bin/moc-qt5)
#
add_executable(Qt5::moc IMPORTED)
	set(imported_location ${path_moc})
	set_target_properties(Qt5::moc PROPERTIES IMPORTED_LOCATION ${imported_location})
	# For CMake automoc feature
	get_target_property(QT_MOC_EXECUTABLE Qt5::moc LOCATION)

# find qt5 core lib
find_package(Qt5Core)
find_package(Qt5WebSockets)


set(Sources
	main.cpp
	lightcontroller.cpp
	)
add_executable(${PROJECT_NAME} ${Sources})

# use the core module of qt5
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::WebSockets)

