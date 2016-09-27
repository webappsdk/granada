{

  name : "sum",

  sum : function(a,b){
    return parseInt(a)+parseInt(b);
  },

  run : function(params){
    var result;
    var message = {};
    sendMessage({"message":"hi"},["math.square","math.multiplication"],function(data){
      message = data;
    });
    result = null;
    if (params["addend1"] && params["addend2"]){
      result = this.sum(params["addend1"],params["addend2"]);
    }
    var oldValue = getValue("sum");
    var response = "{\"configurationtest\":" + JSON.stringify(getConfiguration()) + ",\"result\":" + result + ", \"message\":" + JSON.stringify(message) + ", \"old_value\":\"" + oldValue + "\"}";
    setValue("sum",""+result);
    return response;
  }

}