# node-expat-object

## What?

C based XML -> JS Object conversion

## Why?

Because 3MB SOAP files

## How?

Using [libexpat](http://expat.sourceforge.net/) and some C++ code

## Approaches:
1. Build JS object [in c](https://github.com/dxg/node-expat-json/tree/build-in-c) - fast
2. Build JS object [in js buffer](https://github.com/dxg/node-expat-json/tree/build-in-js-buffer) - faster
3. Build JS object [in js string](https://github.com/dxg/node-expat-json/tree/build-in-js-string) - faster
4. Build JS Object [using JSON.parse](https://github.com/dxg/node-expat-json/tree/build-json) - fastest

## Instructions:
1. Find a suitably large XML file, eg ftp://ftp.archive.de.uu.net/pub/documents/rfc/rfc-index.xml
2. Rename it to `resp.xml`
3. Checkout the branch you want
4. npm install -g node-gyp
5. npm install
6. `node-gyp configure`
7. `node-gyp build`
8. node test.js

You will need to run steps 5-7 every time you change branches.

This package hasn't yet been released to npm. Needs more testing.

Note that the json branch currently crashes with rfc-index.xml because I forgot to escape "

## Thanks

To [node-xmpp/node-expat](https://github.com/node-xmpp/node-expat) which inspired this package.
I copied their DEPS folder.
