{

  name : "square",

  square : function(x){
    return x*x;
  },

  run : function(params){
    var me = this;
    var result;
    var message = {};
    sendMessage({"message":"hi"},[],function(data){
      message = data;
    });
    result = null;
    if (params["number"]){
      result = me.square(params["number"]);
    }
    var oldValue = getValue("number");
    var response = "{\"configurationtest\":" + JSON.stringify(getConfiguration()) + ",\"result\":" + result + ", \"message\":" + JSON.stringify(message) + ", \"old_value\":\"" + oldValue + "\"}";
    setValue("number",""+result);
    return response;
  }

}