var _         = require('lodash');
var fs        = require('fs');
var expat     = require('./lib/node-expat');

var xml = ""
xml += '<box colour="grey">';
xml += '  <book title="Rocket science in a day">';
xml += '    <page number="1"></page>';
xml += '  </book>';
xml += '  <book title="Counting to 10">';
xml += '    <page number="1"></page>';
xml += '    <page number="2"></page>';
xml += '  </book>';
xml += '</box>';

var xml = fs.readFileSync('resp.xml', 'ascii');

console.time("total");
expat.convert(xml, { a: 2, b: 3 }, function (err, data, jsonStr) {
  var json = JSON.parse(jsonStr);

  console.timeEnd("total");

  fs.writeFileSync("out2.json", JSON.stringify(json, null, 2));
});

