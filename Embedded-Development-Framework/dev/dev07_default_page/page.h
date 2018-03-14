

//!!--------------------------------------------------------------

//!! Homepage, 2003 bytes (can be up to 2048 bytes)
const char * indexHtml="<html><head><title>ECC-PIC24-BOARD</title><link rel='icon'href='data:;base64,iVBORw0KGgo='><link rel='stylesheet'href='http://ecc-lab.com/shared/bootstrap.css'><script src='http://ecc-lab.com/shared/jquery.js'></script><script src='http://ecc-lab.com/shared/bootstrap.js'></script><link rel='stylesheet'type='text/css'href='index.css'><script src='index.js'></script></head><body><center><div class='container'><div class='row box'><div class='col col-xs-12'><div class='g-name'>Toggle LED</div><button class='btn btn-warning'id='led0-inv'>LED0 Inv</button><button class='btn btn-warning'id='led1-inv'>LED1 Inv</button><button class='btn btn-warning'id='led2-inv'>LED2 Inv</button><button class='btn btn-warning'id='led3-inv'>LED3 Inv</button></div></div><div class='row box'><div class='col col-xs-12'><div class='g-name'>Torn ON LED</div><button class='btn btn-danger'id='led0-on'>LED0 On</button><button class='btn btn-danger'id='led1-on'>LED1 On</button><button class='btn btn-danger'id='led2-on'>LED2 On</button><button class='btn btn-danger'id='led3-on'>LED3 On</button></div></div><div class='row box'><div class='col col-xs-12'><div class='g-name'>Torn OFF LED</div><button class='btn btn-default'id='led0-off'>LED0 Off</button><button class='btn btn-default'id='led1-off'>LED1 Off</button><button class='btn btn-default'id='led2-off'>LED2 Off</button><button class='btn btn-default'id='led3-off'>LED3 Off</button></div></div><div class='row box'><div class='col col-xs-12'><div class='g-name'>Read ADC</div><button class='btn btn-success'id='adc0'>ADC0</button><button class='btn btn-success'id='adc1'>ADC1</button><button class='btn btn-success'id='adc2'>ADC2</button><button class='btn btn-success'id='adc3'>ADC3</button></div></div><div class='row box'><div class='col col-xs-12'><div class='g-name'id='adc'></div></div></div></div></center></body></html>";

//!! CSS
const char * indexCss=".g-name{color:grey;font-size:1.4em;margin-bottom:0.5em;}.box{border:2px solid gray;border-radius:10px;padding:1.5em;margin:1.5em;}.btn{width:7em;height:3em;font-weight:bold;margin-left:1em;margin-right:1em;}.btn-danger,.btn-warning{color:black;}";

//!! JS
const char * indexJs=" window.onload=function(e){function loadXMLDoc(req){var xmlhttp=new XMLHttpRequest();xmlhttp.onreadystatechange=function(){if (xmlhttp.readyState==XMLHttpRequest.DONE){if (xmlhttp.status==200){document.getElementById('adc').innerHTML=xmlhttp.responseText;}else if (xmlhttp.status==400){console.log('There was an error 400');}else{console.log('something else other than 200 was returned');}}};xmlhttp.open('GET',req,true);xmlhttp.send();}document.getElementById('led0-inv').addEventListener('click',function(){loadXMLDoc('led0-inv');});document.getElementById('led1-inv').addEventListener('click',function(){loadXMLDoc('led1-inv');});document.getElementById('led2-inv').addEventListener('click',function(){loadXMLDoc('led2-inv');});document.getElementById('led3-inv').addEventListener('click',function(){loadXMLDoc('led3-inv');});document.getElementById('led0-on').addEventListener('click',function(){loadXMLDoc('led0-on');});document.getElementById('led1-on').addEventListener('click',function(){loadXMLDoc('led1-on');});document.getElementById('led2-on').addEventListener('click',function(){loadXMLDoc('led2-on');});document.getElementById('led3-on').addEventListener('click',function(){loadXMLDoc('led3-on');});document.getElementById('led0-off').addEventListener('click',function(){loadXMLDoc('led0-off');});document.getElementById('led1-off').addEventListener('click',function(){loadXMLDoc('led1-off');});document.getElementById('led2-off').addEventListener('click',function(){loadXMLDoc('led2-off');});document.getElementById('led3-off').addEventListener('click',function(){loadXMLDoc('led3-off');});document.getElementById('adc0').addEventListener('click',function(){loadXMLDoc('adc0');});document.getElementById('adc1').addEventListener('click',function(){loadXMLDoc('adc1');});document.getElementById('adc2').addEventListener('click',function(){loadXMLDoc('adc2');});document.getElementById('adc3').addEventListener('click',function(){loadXMLDoc('adc3');});}";

//!! favicon.ico
const char *faviconIco = "ECC-Lab";