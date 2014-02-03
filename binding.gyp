{
  'targets': [
    {
      'target_name': 'node_expat_object',
      'sources': [
        'src/parse.cc',
        'src/node-expat-object.cc'
      ],
      'include_dirs': [
        '<!(node -e "require(\'nan\')")'
      ],
      'dependencies': [
        'deps/libexpat/libexpat.gyp:expat'
      ]
    }
  ]
}
