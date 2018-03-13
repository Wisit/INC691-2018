

//!!--------------------------------------------------------------

//!! Homepage, 2003 bytes (can be up to 2048 bytes)

//>> HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n 
const char * indexHtml="HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n <html><head><title>ECC-PIC24-BOARD</title><link rel='icon'href='data:;base64,iVBORw0KGgo='><link rel='stylesheet'href='http://ecc-lab.com/shared/bootstrap.css'><script src='http://ecc-lab.com/shared/jquery.js'></script><script src='http://ecc-lab.com/shared/bootstrap.js'></script><link rel='stylesheet'type='text/css' href='./index.css'><script src='index.js'></script></head><body><center><div class='container'><div class='row box'><div class='col col-xs-12'><div class='g-name'>Toggle LED</div><button class='btn btn-warning'id='led0-inv'>LED0 Inv</button><button class='btn btn-warning'id='led1-inv'>LED1 Inv</button><button class='btn btn-warning'id='led2-inv'>LED2 Inv</button><button class='btn btn-warning'id='led3-inv'>LED3 Inv</button></div></div><div class='row box'><div class='col col-xs-12'><div class='g-name'>Torn ON LED</div><button class='btn btn-danger'id='led0-on'>LED0 On</button><button class='btn btn-danger'id='led1-on'>LED1 On</button><button class='btn btn-danger'id='led2-on'>LED2 On</button><button class='btn btn-danger'id='led3-on'>LED3 On</button></div></div><div class='row box'><div class='col col-xs-12'><div class='g-name'>Torn OFF LED</div><button class='btn btn-primary'id='led0-off'>LED0 Off</button><button class='btn btn-primary'id='led1-off'>LED1 Off</button><button class='btn btn-primary'id='led2-off'>LED2 Off</button><button class='btn btn-primary'id='led3-off'>LED3 Off</button></div></div><div class='row box'><div class='col col-xs-12'><div class='g-name'>Read ADC</div><button class='btn btn-success'id='adc0'>ADC0</button><button class='btn btn-success'id='adc1'>ADC1</button><button class='btn btn-success'id='adc2'>ADC2</button><button class='btn btn-success'id='adc3'>ADC3</button></div></div><div class='row box'><div class='col col-xs-12'><div class='result'id='adc'></div></div></div><div class='row'><div class='col col-xs-12'><br/></div><div class='col col-xs-12'></div></div></div></center></body></html>";



//!! CSS
//>> HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nConnection: close\r\n\r\n 
const char * indexCss="HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nConnection: close\r\n\r\n body{margin:0;padding:0;background-color:#223344;}.g-name{color:grey;font-size:2.0em;margin-top:0.5em;margin-bottom:0.5em;}.box{border:2px solid gray;border-radius:10px;padding-bottom:3em;margin-top:3em;}.btn{width:7em;height:3.5em;font-weight:bold;margin-left:0.5em;margin-right:0.5em;font-size:1.5em;}.btn-danger,.btn-warning{color:black;}.result{font-weight:bold;margin-top:4.7em;}";



//!! JS
//>> HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\n\r\n 
const char * indexJs="HTTP/1.1 200 OK\r\nContent-Type: text/javascript\r\n\r\n window.onload=function(e){function loadXMLDoc(req){var xmlhttp=new XMLHttpRequest();xmlhttp.timeout=2000;xmlhttp.onreadystatechange=function(){if (xmlhttp.readyState==XMLHttpRequest.DONE){if (xmlhttp.status==200){document.getElementById('adc').innerHTML=xmlhttp.responseText;}else if (xmlhttp.status==400){console.log('There was an error 400');}else{console.log('something else other than 200 was returned');}}};xmlhttp.open('GET',req,true);xmlhttp.send();}document.getElementById('led0-inv').addEventListener('click',function(){loadXMLDoc('led/0/2;');});document.getElementById('led1-inv').addEventListener('click',function(){loadXMLDoc('led/1/2;');});document.getElementById('led2-inv').addEventListener('click',function(){loadXMLDoc('led/2/2;');});document.getElementById('led3-inv').addEventListener('click',function(){loadXMLDoc('led/3/2;');});document.getElementById('led0-on').addEventListener('click',function(){loadXMLDoc('led/0/1;');});document.getElementById('led1-on').addEventListener('click',function(){loadXMLDoc('led/1/1;');});document.getElementById('led2-on').addEventListener('click',function(){loadXMLDoc('led/2/1;');});document.getElementById('led3-on').addEventListener('click',function(){loadXMLDoc('led/3/1;');});document.getElementById('led0-off').addEventListener('click',function(){loadXMLDoc('led/0/0;');});document.getElementById('led1-off').addEventListener('click',function(){loadXMLDoc('led/1/0;');});document.getElementById('led2-off').addEventListener('click',function(){loadXMLDoc('led/2/0;');});document.getElementById('led3-off').addEventListener('click',function(){loadXMLDoc('led/3/0;');});document.getElementById('adc0').addEventListener('click',function(){loadXMLDoc('adc/0/0;');});document.getElementById('adc1').addEventListener('click',function(){loadXMLDoc('adc/1/0;');});document.getElementById('adc2').addEventListener('click',function(){loadXMLDoc('adc/2/0;');});document.getElementById('adc3').addEventListener('click',function(){loadXMLDoc('adc/3/0;');});}";


//!! favicon.ico
const char *faviconIco = "ECC-Lab";

















const char * httpOkHeader = "HTTP/1.1 200 OK\r\n\r\n";

const char * htmlHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nServer: ECC-Lab\r\nConnection: close\r\n\r\n";
const char * cssHeader  = "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\nServer: ECC-Lab\r\nConnection:  close\r\n\r\n";

