cmake_minimum_required(VERSION 3.0)
project(ProjetAlgo)
include_directories("/usr/include/SDL2")


# Ajoutez vos fichiers source
set(SOURCES
    ../bidiarray.cc
    ../bidiarray.h
    ../strategy.h
    ../strategy.cc
)

# Ajouter l'exécutable de CollectionParticule
add_executable(test_display test_display.cpp ${SOURCES})

# Ajouter une règle pour exécuter le programme avec des arguments spécifiés
add_custom_target(test_affichage
    COMMAND ./test_display
    DEPENDS test_display
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)