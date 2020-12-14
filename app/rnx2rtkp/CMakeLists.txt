find_package(Qt5 COMPONENTS Core Widgets SerialPort REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PNG libpng16 REQUIRED)

add_executable(rnx2rtkp rnx2rtkp.c)
target_link_libraries(rnx2rtkp RTKLib)