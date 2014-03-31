var _         = require('lodash');
var fs        = require('fs');
var expat     = require('./lib/node-expat');
var templates = require('./templates');

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

function readObject(opts) {
  var a, len, aNameLen, aValueLen, attrCount;
  var obj;
  var name, aName, aValue;

  // node name
  len  = opts.binary.readUInt32LE(opts.binaryPos);
  name = opts.string.substr(opts.stringPos, len)
  opts.binaryPos += 4;
  opts.stringPos += len;

  // find right constructor

  // OPTION 1
  //obj = {};


  // OPTION 2
  //if (name in templates) {
  //  obj = new templates[name];
  //} else {
  //  obj = {};
  //}

  // OPTION 3
  if      (name == "air:AirSegment")         obj = new templates['air:AirSegment'];
  else if (name == "air:FareInfo")           obj = new templates['air:FareInfo'];
  else if (name == "air:AirPricingInfo")     obj = new templates['air:AirPricingInfo'];
  else if (name == "air:AirPricingSolution") obj = new templates['air:AirPricingSolution'];
  else if (name == "air:FareInfoRef")        obj = new templates['air:FareInfoRef'];
  else if (name == "air:BookingInfo")        obj = new templates['air:BookingInfo'];
  else if (name == "air:TaxInfo")            obj = new templates['air:TaxInfo'];
  else if (name == "air:PassengerType")      obj = new templates['air:PassengerType'];
  else if (name == "air:Journey")            obj = new templates['air:Journey'];
  else if (name == "air:AirSegmentRef")      obj = new templates['air:AirSegmentRef'];
  else if (name == "air:LegRef")             obj = new templates['air:LegRef'];
  else obj = {};

  // number of attributes
  attrCount = opts.binary.readUInt32LE(opts.binaryPos);
  opts.binaryPos += 4;

  for(a = 0; a < attrCount; a++) {
    aNameLen  = opts.binary.readUInt32LE(opts.binaryPos);
    opts.binaryPos += 4;
    aValueLen = opts.binary.readUInt32LE(opts.binaryPos);
    opts.binaryPos += 4;

    aName = opts.string.substr(opts.stringPos, aNameLen);
    opts.stringPos += aNameLen;

    aValue = opts.string.substr(opts.stringPos, aValueLen);
    opts.stringPos += aValueLen;

    obj[aName] = aValue;
  }

  return [name, obj];
}

function bufferToObject(buffer) {
  var opts = {
    binaryPos: 0,
    stringPos: 0
  };

  var curr, root = {};
  var obj = {};
  var arr = [];
  var len = 0, nodes = 0;
  var ret, next, name, node, str;
  var stack = [];

  console.time("js split");

  // split buffer into string & binary parts
  len = buffer.readUInt32LE(0);
  opts.binaryPos += 4;
  opts.string = buffer.toString('ascii', len);
  opts.binary = buffer.slice(0, len);
  buffer = null;

  console.timeEnd("js split");

  // read number of nodes
  nodes = opts.binary.readUInt32LE(opts.binaryPos);
  opts.binaryPos += 4;

  console.log("js number of nodes:", nodes);

  curr = root;

  console.time("js build");

  do {
    do {
      next = opts.binary.readUInt8(opts.binaryPos);
      opts.binaryPos += 1;

      if (next == 2) {
        stack.push(curr);
        curr = node;
      } else if (next == 3) {
        curr = stack.pop();
      }

    } while (next > 1);
    if (next == 0) break;

    ret = readObject(opts);

    name = ret[0];
    node = ret[1];

    curr[name] = curr[name] || [];
    curr[name].push(node);

  } while (true);

  console.timeEnd("js build");
  opts = null;

  return root;
}

console.time("## TOTAL");
expat.convert(xml, { a: 2, b: 3 }, function (err, data, buffer) {
  root = bufferToObject(buffer);
  console.timeEnd("## TOTAL");

  str = JSON.stringify(root, null, 2);

  fs.writeFileSync('out.json', str);

  console.time("js json.parse");
  obj = JSON.parse(str);
  console.timeEnd("js json.parse");
});

