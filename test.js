var async         = require('async');
var fs            = require('fs');
var nodeExpatJson = require('./lib/node-expat');
var xml2json      = require('xml2json');


var xml = ""
xml += '<box colour="grey">';
xml += '  <book title="Rocket science in a day">';
xml += '    <page number="1"></page>';
xml += '  </book>';
xml += '  <book title="Counting to 10">';
xml += '    <page number="1"></page>';
xml += '    <page number="2">abcd</page>';
xml += '  </book>';
xml += '</box>';

var xml = fs.readFileSync('resp.xml', 'utf8');

function printTitle(title) {
  console.log("\n\n---------------");
  console.log(title);
  console.log(    "---------------\n");
}

var tests = [
 {
    name: "node-expat-json",
    run: function (time, cb) {
      nodeExpatJson.convert(xml, { a: 2, b: 3 }, function (err, jsonStr) {
        //console.time("  json.parse");
        var json = JSON.parse(jsonStr);
        //console.timeEnd("  json.parse");

        cb(null, json);
      });
    }
  },
  {
    name: "xml2json",
    run: function (time, cb) {
      var json = xml2json.toJson(xml, {
        object: true, arrayNotation: true, coerce: false,
        sanitize: false, reversible: true, trim: false
      });

      cb(null, json);
    }
  }
];

function start() {
  var numRuns = 1;

  async.eachSeries(tests, function (test, cb) {
    printTitle(`${numRuns} runs of ${test.name}`);
    console.time("# Total");
    async.times(numRuns, test.run, function (err, data) {
      console.timeEnd("# Total");
      if (err) return cb(err);

      fs.writeFile(`out-${test.name}.json`, JSON.stringify(data, null, 2), cb);

      cb();
    });
  }, function (err) {
    if (err) throw err;
    console.log("\n\nDone.");
  });
}

start();
