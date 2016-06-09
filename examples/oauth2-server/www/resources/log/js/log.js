function log(json){
  var logEl = document.getElementById("log");
  var color = "#00ff00";
  var message = "";

  if (json){
    if (json["error"]){
      message = json["error_description"];
      color = "#ee2c2c";
    }else{
      message = json["description"];
      color = "#00ff00";
    }
  }else{
    if (getParameterByName("error") == ""){
      message = getParameterByName("description");
      if (message.length > 0){
        color = "#52de2f";
      }
    }else{
      message = getParameterByName("error_description");
      if (message.length > 0){
        color = "#ee2c2c";
      }
    }
  }
  logEl.style["background-color"] = "#000";
  logEl.style["font-weight"] = "bold";
  logEl.style["color"] = color;
  if (!message){
    message = "";
  }
  logEl.innerHTML = "" + message;
}

function getParameterByName(name, url) {
  if (!url){
    url = window.location.href;
  }

  name = name.replace(/[\[\]]/g, "\\$&");
  var regex = new RegExp("[?&]" + name + "(=([^&#]*)|&|#|$)"),
      results = regex.exec(url);
  if (!results || !results[2]){
    return "";
  }

  return decodeURIComponent(results[2].replace(/\+/g, " "));
};
