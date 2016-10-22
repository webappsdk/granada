var __PLUGIN_ID = "{{__PLUGIN_ID}}";
var __PLUGIN_HANDLER_ID = "{{__PLUGIN_HANDLER_ID}}";


function getConfiguration(){
  return __PLUGIN.configuration_;
};


function setValue(key,value){
  if (key && value && typeof key == "string" && typeof value == "string"){
    var value = __setValue("{\"key\":\"" + key + "\",\"value\":\"" + value + "\",\"__PLUGIN_ID\":\"" + __PLUGIN_ID + "\",\"__PLUGIN_HANDLER_ID\":\"{{__PLUGIN_HANDLER_ID}}\"}");
    var value_json;
    try{
      value_json = JSON.parse(value);
    }catch(e){
      value_json = {"error":"server_error"};
    }
    if (!value_json["error"]){
      return true;
    }
  }
  return false;
};

function getValue(key){
  if (key && typeof key == "string"){
    var value = __getValue("{\"key\":\"" + key + "\",\"__PLUGIN_ID\":\"" + __PLUGIN_ID + "\",\"__PLUGIN_HANDLER_ID\":\"{{__PLUGIN_HANDLER_ID}}\"}");
    var value_json;
    try{
      value_json = JSON.parse(value);
    }catch(e){
      value_json = {"error":"server_error"};
    }
    if (!value_json["error"] && value_json["value"]){
      return value_json["value"];
    }
  }
  return "";
};

function destroyValue(key){
  if (key && typeof key == "string"){
    __destroyValue("{\"key\":\"" + key + "\",\"__PLUGIN_ID\":\"" + __PLUGIN_ID + "\",\"__PLUGIN_HANDLER_ID\":\"{{__PLUGIN_HANDLER_ID}}\"}");
  }
};

function clearValues(){
  __clearValues("{\"__PLUGIN_ID\":\"" + __PLUGIN_ID + "\",\"__PLUGIN_HANDLER_ID\":\"{{__PLUGIN_HANDLER_ID}}\"}");
};

function sendMessage(params, toIds, callback){
  var stringifiedParams;
  var stringifiedToIds;
  var errorMessage = "";
  try{
    stringifiedParams = JSON.stringify(params);
  }catch(e){
    stringifiedParams = null;
    errorMessage = e.message;
  }
  if (stringifiedParams == null){
    if (callback){
      callback({"error":"message_parameters_error","error_description":errorMessage});
    }
  }else{
    var responseData = {};
    try{
      if (toIds==null){
        stringifiedToIds = "[]";
      }else{

        if (toIds.constructor === Array){
          stringifiedToIds = JSON.stringify(toIds);
          var len = toIds.length;
          if (len > 0 && typeof __PLUGINS !== "undefined"){
            // check if ids are on __PLUGINS
            var IdsNotInPlugins = [];
            while(len--){
              var pluginId = toIds[len];
              var plugin = __PLUGINS[pluginId];
              if (plugin && plugin["onMessage"] && typeof plugin["onMessage"] == "function"){
                responseData[pluginId] = __sendIntraMessage(params,pluginId,__PLUGIN_ID);
              }else{
                IdsNotInPlugins.push(toIds[len]);
              }
            }
            if (IdsNotInPlugins.length>0){
              stringifiedToIds = JSON.stringify(IdsNotInPlugins);
            }else{
              var responseJSON = {"data":responseData};
              callback(responseJSON);
              return;
            }
          }else{
            stringifiedToIds = JSON.stringify(toIds);
          }
        }else{
          stringifiedToIds = "[]";
        }
      }
    }catch(e){
      stringifiedToIds = null;
      errorMessage = e.message;
    }
    if (stringifiedToIds == null){
      if (callback){
        callback({"error":"message_to_ids_error","error_description":errorMessage});
      }
    }else{
      var response = __sendMessage("{\"parameters\":" + stringifiedParams + ",\"to_ids\":" + stringifiedToIds +",\"__PLUGIN_ID\":\"" + __PLUGIN_ID + "\",\"__PLUGIN_HANDLER_ID\":\"{{__PLUGIN_HANDLER_ID}}\"}");
      if (callback){
        var responseJSON;
        try{
          responseJSON = JSON.parse(response);
        }catch(e){
          responseJSON = {"data":{}};
        }
        if (responseJSON["data"]){
          for (var key in responseData){
            responseJSON["data"][key] = responseData[key];
          }
        }
        callback(responseJSON);
      }
    }
  }
};

function remove(pluginId){
  if (pluginId){
    if (typeof pluginId == "string"){
      __remove("{\"id\":\"" + pluginId + "\",\"__PLUGIN_HANDLER_ID\":\"{{__PLUGIN_HANDLER_ID}}\"}");
    }
  }else{
    __remove("{\"id\":\"" + __PLUGIN_ID + "\",\"__PLUGIN_HANDLER_ID\":\"{{__PLUGIN_HANDLER_ID}}\"}");
  }
};

function removeEvents(){
  __removeEvents("{\"__PLUGIN_ID\":\"" + __PLUGIN_ID + "\",\"__PLUGIN_HANDLER_ID\":\"{{__PLUGIN_HANDLER_ID}}\"}");
};

function call(name,params){
  var fn = __PLUGIN[name];
  if (fn && __type(fn) == "function"){
    fn(params);
  }
};

function __sendIntraMessage(params,toId,fromId){
  if (typeof __PLUGINS !== "undefined"){
    var plugin = __PLUGINS[toId];
    if (plugin && plugin["onMessage"] && typeof plugin["onMessage"] == "function"){
      var message = null;
      if (params["message"]){
        message = params["message"];
      }
      var responseData = plugin.onMessage(message,fromId);
      if (responseData["error"]){
        return responseData;
      }else{
        return {"data":responseData};
      }
    }
  }else{
    return {"error":"undefined_plugins","error_description":"There are no __PLUGINS."};
  }
};

function __onMessage(message,_from){
  __setPluginConfiguration();

  var returnData;
  if (__PLUGIN["onMessage"] && __type(__PLUGIN["onMessage"]) == "function"){
    returnData = JSON.stringify(__PLUGIN.onMessage(message,_from));
  }else{
    returnData = "{\"error\":\"undefined_onmessage_function\",\"error_description\":\"Undefined onMessage function\"}";
  }
  return returnData;
};


function __multiOnMessage(message,_from){

  __setPluginsConfigurations();

  var response = "{";
  var i = 0;
  var returnData;
  for (var pluginId in __PLUGINS){
    __PLUGIN_ID = pluginId;
    __PLUGIN = __PLUGINS[pluginId];

    if (i>0){ response += ","; }
    if (__PLUGIN["onMessage"] && __type(__PLUGIN["onMessage"]) == "function"){
      var messageResponse = __PLUGIN.onMessage(message,_from);

      if (!messageResponse["error"]){
        try{
          returnData = JSON.stringify(messageResponse);
          response += "\"" + pluginId + "\":{\"data\":" + returnData + "}";
        }catch(e){}
      }
    }
    i++;
  }

  response += "}";

  return response;
};

function fire(eventName,params,success,failure){
  var stringifiedParams;
  var errorMessage = "";
  try{
    stringifiedParams = JSON.stringify(params);
  }catch(e){
    stringifiedParams = null;
    errorMessage = e.message;
  }
  if (stringifiedParams == null){
    if (failure){
      failure({"error":"message_parameters_error","error_description":errorMessage});
    }
  }
  var response = __fire("{\"parameters\":" + stringifiedParams + ",\"event_name\":\"" + eventName + "\",\"__PLUGIN_ID\":\"" + __PLUGIN_ID + "\",\"__PLUGIN_HANDLER_ID\":\"{{__PLUGIN_HANDLER_ID}}\"}");
  if (response["error"]){
    if (failure){
      failure(response);
    }
  }else{
    if (success){
      success(response);
    }
  }
};

function runPlugin(id,params,success,failure){
  var stringifiedParams;
  var errorMessage = "";
  try{
    stringifiedParams = JSON.stringify(params);
  }catch(e){
    stringifiedParams = null;
    errorMessage = e.message;
  }
  if (stringifiedParams == null){
    if (callback){
      callback({"error":"message_parameters_error","error_description":errorMessage});
    }
  }
  var response = __runPlugin("{\"parameters\":" + stringifiedParams + ",\"id\":\"" + id + "\",\"__PLUGIN_ID\":\"" + __PLUGIN_ID + "\",\"__PLUGIN_HANDLER_ID\":\"{{__PLUGIN_HANDLER_ID}}\"}");
  if (response["error"]){
    if (failure){
      failure(response);
    }
  }else{
    if (success){
      success(response);
    }
  }
};


function __getResponseFirstElementData(data){
  if (data){
    var data = data["data"];
    if (data && __type(data) == "object"){
      for (var key in data){
        var pluginData = newData[key];
        if (!pluginData["error"]){
          return pluginData;
        }
      }
    }
  }
  return null;
};


function __wrappedRun(params,eventName){
  __setPluginConfiguration();

  fire(__PLUGIN_ID + "-before",params,function(data){
    // get first result.
    data = __getResponseFirstElementData(data);
    if (data){
      params = data;
    }
  },function(data){});


  fire(__PLUGIN_ID+"-in-process",params);
  var data = __PLUGIN.run(params,eventName);


  fire(__PLUGIN_ID + "-after",params,function(_data){
    // get first result.
    _data = __getResponseFirstElementData(_data);
    if (_data){
      data = _data;
    }
  },function(data){});

  return data;

};



function __runPlugins(params,eventName){
  
  __setPluginsConfigurations();

  
  var response = "{";
  var i = 0;
  for (var pluginId in __PLUGINS){
    __PLUGIN_ID = pluginId;
    __PLUGIN = __PLUGINS[pluginId];

    if (i>0){ response += ","; }
    response += "\"" + pluginId + "\":{\"data\":" + __wrappedRun(params,eventName) + "}";
    i++;
  }

  response += "}";

  return response;
};


function __setPluginConfiguration(){
  if (!__PLUGIN["configuration_"]){
    __PLUGIN["configuration_"] = {{__PLUGIN_CONFIGURATION}};
  }
};


function __setPluginsConfigurations(){
  // set configurations
  var configuration = null;
  for (var pluginId in __PLUGINS){
    if (!__PLUGINS[pluginId]["configuration_"]){
      configuration = __PLUGINS_CONFIGURATIONS[pluginId];
      if (configuration){
        __PLUGINS[pluginId]["configuration_"] = configuration;
      }
    }
  }
};



function __type(obj) {
  if (Array.isArray(obj)){
    return "array";
  } else if (typeof obj == "string"){
    return "string";
  } else if (obj != null && typeof obj == "object"){
    return "object";
  } else if (typeof obj == "function" ){
    return "function";
  }else{
    return "other";
  }
};