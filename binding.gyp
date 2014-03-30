{
  'targets': [
    {
      'target_name': 'node_expat_json',
      'sources': [
        'src/parse.cc',
        'src/node-expat-json.cc'
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
