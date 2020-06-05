import qbs 1.0

Product {
    name:   'fundament'
    type:   'dynamiclibrary'

    files: [
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
            'private/window_win32.c',
            'private/fundament.rc'
        ])

        cpp.staticLibraries: outer.concat([
            'user32'
        ])
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
