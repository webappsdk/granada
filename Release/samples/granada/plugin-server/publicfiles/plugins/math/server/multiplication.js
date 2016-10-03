{

  name : "multiplication",

  multiplication : function(a,b){
    return parseInt(a)*parseInt(b);
  },

  run : function(params){
    var me = this;
    var result;
    var message = {};
    sendMessage({"message":"hi"},[],function(data){
      message = data;
    });
    result = null;
    if (params["factor1"] && params["factor2"]){
      result = me.multiplication(params["factor1"],params["factor2"]);
    }
    var oldValue = getValue("sum");
    var response = "{\"result\":" + result + ", \"message\":" + JSON.stringify(message) + ", \"old_value\":\"" + oldValue + "\"}";
    setValue("sum",""+result);
    return response;
  }

}