
window.onload = function(e){ 
    function loadXMLDoc(req) {
        
        var xmlhttp = new XMLHttpRequest();
        xmlhttp.timeout = 2000;

        xmlhttp.onreadystatechange = function() {
            if (xmlhttp.readyState == XMLHttpRequest.DONE) {
            if (xmlhttp.status == 200) {
                document.getElementById('adc').innerHTML = xmlhttp.responseText;
            }
            else if (xmlhttp.status == 400) {
                console.log('There was an error 400');
            }
            else {
                console.log('something else other than 200 was returned');
            }
            }
        };
        xmlhttp.open("GET", req, true);
        xmlhttp.send();
    }

    document.getElementById('led0-inv').addEventListener("click", function(){
        loadXMLDoc('led/0/2;');
    });
    document.getElementById('led1-inv').addEventListener("click", function(){
        loadXMLDoc('led/1/2;');
    });
    document.getElementById('led2-inv').addEventListener("click", function(){
        loadXMLDoc('led/2/2;');
    });
    document.getElementById('led3-inv').addEventListener("click", function(){
        loadXMLDoc('led/3/2;');
    });

    document.getElementById('led0-on').addEventListener("click", function(){
        loadXMLDoc('led/0/1;');
    });
    document.getElementById('led1-on').addEventListener("click", function(){
        loadXMLDoc('led/1/1;');
    });
    document.getElementById('led2-on').addEventListener("click", function(){
        loadXMLDoc('led/2/1;');
    });
    document.getElementById('led3-on').addEventListener("click", function(){
        loadXMLDoc('led/3/1;');
    });

    document.getElementById('led0-off').addEventListener("click", function(){
        loadXMLDoc('led/0/0;');
    });
    document.getElementById('led1-off').addEventListener("click", function(){
        loadXMLDoc('led/1/0;');
    });
    document.getElementById('led2-off').addEventListener("click", function(){
        loadXMLDoc('led/2/0;');
    });
    document.getElementById('led3-off').addEventListener("click", function(){
        loadXMLDoc('led/3/0;');
    });

    document.getElementById('adc0').addEventListener("click", function(){
        loadXMLDoc('adc/0/0;');
    });
    document.getElementById('adc1').addEventListener("click", function(){
        loadXMLDoc('adc/1/0;');
    });
    document.getElementById('adc2').addEventListener("click", function(){
        loadXMLDoc('adc/2/0;');
    });
    document.getElementById('adc3').addEventListener("click", function(){
        loadXMLDoc('adc/3/0;');
    });
}
