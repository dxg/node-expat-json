# node-expat-object

## What?

C based XML -> JS Object conversion

## Why?

Because 3MB SOAP files

## How?

Using [libexpat](http://expat.sourceforge.net/) and some C++ code

## Approaches:
1. Build JS object [in c](https://github.com/dxg/node-expat-json/tree/build-in-c) - fast
2. Build JS object [in js buffer](https://github.com/dxg/node-expat-json/tree/build-in-js-string) - faster
3. Build JS object [in js string](https://github.com/dxg/node-expat-json/tree/build-in-js-buffer) - faster
4. Build JS Object [using JSON.parse](https://github.com/dxg/node-expat-json/tree/build-json) - fastest

## Thanks

To [node-xmpp/node-expat](https://github.com/node-xmpp/node-expat) which inspired this package.
I copied their DEPS folder.
