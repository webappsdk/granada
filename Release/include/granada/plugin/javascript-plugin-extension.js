
function __extend(){
  var pluginExtends = __PLUGIN_JSON["extends"];

  var plugin;
  var pluginJSONConfiguration = __PLUGIN_JSON["configuration_"];
  if (!pluginJSONConfiguration || type(pluginJSONConfiguration) != "object"){
	pluginJSONConfiguration = {{__PLUGIN_CONFIGURATION}};
  };

  for (var pluginId in __EXTENDED_PLUGINS){
    plugin = __EXTENDED_PLUGINS[pluginId];

    // extend plug-in members
    for (var memberName in plugin){
      if(typeof __PLUGIN_JSON[memberName] == "undefined"){
        __PLUGIN_JSON[memberName] = plugin[memberName];
      }
    };

    // extend plug-in configuration
    if (__EXTENDED_CONFIGURATIONS){
    	var configuration = __EXTENDED_CONFIGURATIONS[pluginId];
    	if (configuration){
	    	for (var key in __EXTENDED_CONFIGURATIONS){
	    		if(typeof pluginJSONConfiguration[key] == "undefined"){
		        	pluginJSONConfiguration[key] = configuration[key];
		      	}
	    	}
    	}
    };

    pluginToExtendExtends = plugin["extends"];
    if(__type(pluginToExtendExtends) == "array"){
		// concatenate extends.
		var len = pluginToExtendExtends.length;
		while (len--){
			var extendedId = pluginToExtendExtends[len];
			if (pluginExtends.indexOf(extendedId) == -1){
				pluginExtends.push(extendedId);
			}
		}
	}
  };

  __PLUGIN_JSON["configuration_"] = pluginJSONConfiguration;

  __PLUGIN_JSON["extends"] = pluginExtends;

};


function __print(obj){
	var stringifiedObj = "{";
	var item;
	var i = 0;
	for (var key in obj){
		var stringifiedItem = false;
		item = obj[key];
		var type = __type(item);
		if (type == "string"){
			if (i>0){stringifiedObj+=",";}
			stringifiedObj += "\"" + key + "\":\"" + item + "\"";
			stringifiedItem = true;
		}else if (type == "object"){
			if (i>0){stringifiedObj+=",";}
			stringifiedObj += "\"" + key + "\":" + __print(item);
		    stringifiedItem = true;
		}else if (type == "function"){
			if (i>0){stringifiedObj+=",";}
			stringifiedObj += "\"" + key + "\":" + item;
			stringifiedItem = true;
		}

		if (stringifiedItem){
			i++;
		}
	}
	stringifiedObj += "}";
	return stringifiedObj;
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