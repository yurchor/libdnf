ADD_SUBDIRECTORY (solvable)

SET (REPO_SOURCES
        ${REPO_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/DependencySplitter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Repo.cpp
        PARENT_SCOPE
    )

SET (REPO_HEADERS
        ${REPO_HEADERS}
        ${CMAKE_CURRENT_SOURCE_DIR}/Repo.hpp
        PARENT_SCOPE
    )
