///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Bookmarklet: formata json e exibe em uma janela flutuante
//
///////////////////////////////////////////////////////////////////////////////////////////////////

javascript:(function() {
    function se(d) {
        return d.selection ? d.selection.createRange().text : d.getSelection()
    } 
    s = se(document); 
    for (i=0; i<frames.length && !s; i++) s = se(frames[i].document);
    if (!s || s=='') s = prompt('Enter json','');
    var id = 'ContainerJSON';
    var div = document.getElementById(id);
    var body = document.getElementsByTagName('body')[0];
    if (null != div) {
       body.removeChild(div);
     }
    div = document.createElement('pre');
    div.id = id;
    div.style.width = 'auto';
    div.style.minWidth = '20px';
    div.style.height = 'auto';
    div.style.minHeight = '20px';
    div.style.fontSize = '14px';
    div.style.position = 'fixed';
    div.style.top = '10px';
    div.style.left = '10px';
    div.style.background = 'white';
    div.style.color = 'black';
    div.style.borderColor = 'black';
    div.style.borderStyle = 'solid';
    div.style.borderWidth = '2px';
    div.style.zIndex = '99999999';
    div.style.padding = '16px';
		div.style.overflowY = 'auto';
		div.style.maxHeight = '85%';
    div.innerHTML = JSON.stringify(JSON.parse(s),0,2);
    div.onclick = function() { body.removeChild(div); };
    body.appendChild(div);
})();

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Fonte: tinderizer
//
///////////////////////////////////////////////////////////////////////////////////////////////////
javascript:document.write(
'<textarea%20cols=150%20rows=40%20id=t></textarea><br/><button%20onclick="t.value=JSON.stringify(JSON.parse(t.value),0,2)">Prettify!</button>');


javascript:(function() {       
var setupDiv = function() {
 var id = 'Tinderizer';
 var div = document.getElementById(id);
 var body = document.getElementsByTagName('body')[0];
 if (null != div) {           
           body.removeChild(div);
 }         
 div = document.createElement('div');
 div.id = id;
 div.style.width = 'auto';
 div.style.height = '30px';
 div.style.fontSize = '14px';
 div.setAttribute('data-email', 'joaopedro6br@kindle.com');
 div.style.position = 'fixed';
 div.style.top = '10px';
 div.style.left = '10px';
 div.style.background = 'white';
 div.style.color = 'black';
 div.style.borderColor = 'black';
 div.style.borderStyle = 'solid';
 div.style.borderWidth = '2px';
 div.style.zIndex = '99999999';
 div.style.padding = '16px';
 div.style.textAlign = 'center';
 div.innerHTML = 'Working...';
 body.appendChild(div);
};
setupDiv();
var script = document.createElement('script');
script.type = 'text/javascript';
script.src = '//tinderizer.com/static/bookmarklet.js?t='   (new Date()).getTime();
document.getElementsByTagName('head')[0].appendChild(script);
     })();
