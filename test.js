var _         = require('lodash');
var fs        = require('fs');
var expat     = require('./lib/node-expat');
var jsParser  = require('xml2json');

//var xml = ""
//xml += '<box colour="grey">';
//xml += '  <book title="Rocket science in a day">';
//xml += '    <page number="1"></page>';
//xml += '  </book>';
//xml += '  <book title="Counting to 10">';
//xml += '    <page number="1"></page>';
//xml += '    <page number="2"></page>';
//xml += '  </book>';
//xml += '</box>';

var xml = fs.readFileSync('resp.xml', 'ascii');


console.time("C conversion total")
expat.convert(xml, { someOption: 5 }, function (err, data, object) {
  console.timeEnd("C conversion total")

  console.time("js+c sax parser conversion")
  json = jsParser.toJson(xml, { object: true, arrayNotation: true, coerce: false, sanitize: false, reversible: true });
  console.timeEnd("js+c sax parser conversion")
});
