var nodeExpatObj = require('../build/Release/node_expat_json.node');


module.exports = {
  convert: function (xml, opts, cb) {
    nodeExpatObj.convert(xml, opts, cb);
  }
};
