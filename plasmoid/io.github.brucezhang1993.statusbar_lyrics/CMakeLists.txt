cmake_minimum_required(VERSION 3.16)

project(io.github.brucezhang1993.status_lyrics)

find_package(ECM 1.4.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(KF5 REQUIRED COMPONENTS
    Plasma # Required for plasma_install_package()
)

plasma_install_package(package io.github.brucezhang1993.status_lyrics)
