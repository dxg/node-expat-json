
var constructors = {};


// "air:AirSegment"
constructors['air:AirSegment'] = function () {
  this["ArrivalTime"] = null;
  this["AvailabilitySource"] = null;
  this["Carrier"] = null;
  this["ChangeOfPlane"] = null;
  this["DepartureTime"] = null;
  this["Destination"] = null;
  this["Distance"] = null;
  this["ETicketability"] = null;
  this["Equipment"] = null;
  this["FlightNumber"] = null;
  this["FlightTime"] = null;
  this["Group"] = null;
  this["Key"] = null;
  this["OptionalServicesIndicator"] = null;
  this["Origin"] = null;
  this["ParticipantLevel"] = null;
  this["PolledAvailabilityOption"] = null;
  this["$t"] = null;
  this["air:CodeshareInfo"] = null;
  this["air:AirAvailInfo"] = null;
  this["air:FlightDetailsRef"] = null;
};

// "air:AirSegmentRef"
constructors['air:AirSegmentRef'] = function () {
  this["Key"] = null;
};

// "air:FareInfo"
constructors['air:FareInfo'] = function () {
  this["Amount"] = null;
  this["Destination"] = null;
  this["EffectiveDate"] = null;
  this["FareBasis"] = null;
  this["FareFamily"] = null;
  this["Key"] = null;
  this["Origin"] = null;
  this["PassengerTypeCode"] = null;
  this["PromotionalFare"] = null;
};

// "air:AirPricingInfo"
constructors['air:AirPricingInfo'] = function () {
  this["ApproximateBasePrice"] = null;
  this["ApproximateTotalPrice"] = null;
  this["BasePrice"] = null;
  this["Key"] = null;
  this["PricingMethod"] = null;
  this["ProviderCode"] = null;
  this["SupplierCode"] = null;
  this["Taxes"] = null;
  this["TotalPrice"] = null;
  this["$t"] = null;
  this["air:FareInfoRef"] = null;
  this["air:BookingInfo"] = null;
  this["air:TaxInfo"] = null;
  this["air:PassengerType"] = null;
};

// "air:AirPricingSolution"
constructors['air:AirPricingSolution'] = function () {
  this["ApproximateBasePrice"] = null;
  this["ApproximateTotalPrice"] = null;
  this["BasePrice"] = null;
  this["Key"] = null;
  this["Taxes"] = null;
  this["TotalPrice"] = null;
};

// "air:FareInfoRef"
constructors['air:FareInfoRef'] = function () {
  this["Key"] = null;
};

// "air:BookingInfo"
constructors['air:BookingInfo'] = function () {
  this["BookingCode"] = null;
  this["CabinClass"] = null;
  this["FareInfoRef"] = null;
  this["HostTokenRef"] = null;
  this["SegmentRef"] = null;
};

// "air:TaxInfo"
constructors['air:TaxInfo'] = function () {
  this["Amount"] = null;
  this["Category"] = null;
};

// "air:PassengerType"
constructors['air:PassengerType'] = function () {
  this["Code"] = null;
};

// "air:Journey"
constructors['air:Journey'] = function () {
  this["TravelTime"] = null;
  this["$t"] = null;
};

// "air:LegRef"
constructors['air:LegRef'] = function () {
  this["Key"] = null;
}

//var test = function () {
//  this["air:AirSegment"] = constructors["air:AirSegment"];
//  this["air:AirSegmentRef"] = constructors["air:AirSegmentRef"];
//  this["air:FareInfo"] = constructors["air:FareInfo"];
//  this["air:AirPricingInfo"] = constructors["air:AirPricingInfo"];
//  this["air:AirPricingSolution"] = constructors["air:AirPricingSolution"];
//  this["air:FareInfoRef"] = constructors["air:FareInfoRef"];
//  this["air:BookingInfo"] = constructors["air:BookingInfo"];
//  this["air:TaxInfo"] = constructors["air:TaxInfo"];
//  this["air:PassengerType"] = constructors["air:PassengerType"];
//  this["air:Journey"] = constructors["air:Journey"];
//  this["air:LegRef"] = constructors["air:LegRef"];
//}


//module.exports = new test();
module.exports = constructors;
