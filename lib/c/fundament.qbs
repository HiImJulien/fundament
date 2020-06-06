import qbs 1.0

Product {
    name:   'fundament'
    type:   'dynamiclibrary'

    files: [
        'private/input_common.c',
        'private/input.c',
        'private/window_common.c',
        'private/window.c'
    ]

    Depends{ name: 'cpp' }
    cpp.cLanguageVersion:   'c11'
    cpp.includePaths:       'public'
    cpp.defines:            'FUNDAMENT_EXPORTS'

    Properties{
        condition: qbs.targetOS.contains('windows')

        files: outer.concat([
            'private/fundament.rc',
            'private/input_win32.c',
            'private/window_win32.c',
        ])

        cpp.staticLibraries: outer.concat([
            'user32'
        ])
    }

    Properties{
        condition: qbs.targetOS.contains('macos')

        files: outer.concat([
            'private/window_AppKit.m'
        ])

        cpp.frameworks: [
            'AppKit'
        ]
    }

    Export {
        Depends{name: 'cpp' }
        cpp.includePaths:   'public'
    }

    Group {
        qbs.install:    true
        fileTagsFilter: product.type
    }
}
