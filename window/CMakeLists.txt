
add_library(
    fundament_window
    STATIC
)

target_sources(
    fundament_window
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/input.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window_common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/input.h
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/window.h
)

target_include_directories(
    fundament_window
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
)

target_link_libraries(
    fundament_window
    PUBLIC
        fundament::core
)

add_library(
    fundament::window
    ALIAS
    fundament_window
)

if(FN_TARGET_IS_MACOS)
    target_sources(
        fundament_window
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/cocoa/cocoa_window.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/cocoa/cocoa_window.m
            ${CMAKE_CURRENT_SOURCE_DIR}/private/cocoa/cocoa_window_delegate.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/cocoa/cocoa_window_delegate.m
    )

    target_link_libraries(
        fundament_window
        PUBLIC
        "-framework AppKit"
    )
    
    target_compile_features(
        fundament_window
        PRIVATE
            c_std_11
    )
endif()

if(${FN_TARGET_IS_WIN32})
    target_sources(
        fundament_window
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/win32/win32_window.c
            ${CMAKE_CURRENT_SOURCE_DIR}/private/win32/win32_window.h
    )

    target_link_libraries(
        fundament_window
        PUBLIC
            user32
    )
endif()

if(${FN_TARGET_IS_LINUX})
    target_sources(
        fundament_window
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/xcb/xcb_window.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/xcb/xcb_window.c 
            ${CMAKE_CURRENT_SOURCE_DIR}/private/xcb/xcb_window_map.c
            ${CMAKE_CURRENT_SOURCE_DIR}/private/xcb/xcb_window_map.h
    )

    pkg_check_modules(xcb REQUIRED IMPORTED_TARGET xcb)
    pkg_check_modules(wayland-client IMPORTED_TARGET wayland-client)

    target_link_libraries(
        fundament_window
        PRIVATE
            PkgConfig::xcb
    )

    if(TARGET PkgConfig::wayland-client)
        target_sources(
            fundament_window
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/private/wayland/wayland_window.h
                ${CMAKE_CURRENT_SOURCE_DIR}/private/wayland/wayland_window.c
                ${CMAKE_CURRENT_SOURCE_DIR}/private/wayland/wayland_decorations.h 
                ${CMAKE_CURRENT_SOURCE_DIR}/private/wayland/wayland_decorations.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/private/wayland/xdg-shell.h
                ${CMAKE_CURRENT_SOURCE_DIR}/private/wayland/xdg-shell.c
                ${CMAKE_CURRENT_SOURCE_DIR}/private/backend/window_backend.h
                ${CMAKE_CURRENT_SOURCE_DIR}/private/backend/window_backend.c
        )
    
        target_link_libraries(
            fundament_window
            PRIVATE
                PkgConfig::wayland-client
                rt
                m
        )

        target_compile_definitions(
            fundament_window
            PRIVATE
                FN_HAS_WAYLAND
        )
    endif() 
endif()

