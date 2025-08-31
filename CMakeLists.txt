cmake_minimum_required(VERSION 3.21)
project(PagosApp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql Core)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt_add_executable(pagos
    src/main.cpp
    src/ui/MainWindow.cpp src/ui/MainWindow.h
    src/data/Database.cpp src/data/Database.h
    src/data/Migrations.cpp src/data/Migrations.h
    src/LicenseManager.cpp src/LicenseManager.h
    src/data/repository/StudentRepository.cpp src/data/repository/StudentRepository.h
    src/data/repository/PaymentRepository.cpp src/data/repository/PaymentRepository.h
)

# Para que los #include vean headers sin poner rutas relativas
target_include_directories(pagos PRIVATE
    src
    src/ui
    src/data
    src/data/repository
)

target_link_libraries(pagos PRIVATE Qt6::Widgets Qt6::Sql Qt6::Core)

add_executable(licgen
    tools/LicGen.cpp
    src/LicenseManager.cpp
    src/data/Database.cpp
    src/data/Database.h
)
target_include_directories(licgen PRIVATE src src/data)
target_link_libraries(licgen PRIVATE Qt6::Core Qt6::Sql)

