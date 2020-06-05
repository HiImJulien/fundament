import qbs 1.0

Product {
    name: 'simple_window'
    type: 'application'

    Depends{ name: 'cpp' }
    Depends{ name: 'fundament' }

    files: [
        'simple_window.c'
    ]

    Group {
        qbs.install:    true
        fileTagsFilter: product.type
    }
}