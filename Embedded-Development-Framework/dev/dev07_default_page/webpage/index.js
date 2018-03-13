
window.onload = function(e){ 
    function loadXMLDoc(req) {
        
        var xmlhttp = new XMLHttpRequest();

        xmlhttp.onreadystatechange = function() {
            if (xmlhttp.readyState == XMLHttpRequest.DONE) {
            if (xmlhttp.status == 200) {
                document.getElementById("adc").innerHTML = xmlhttp.responseText;
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
        loadXMLDoc('led0-inv');
    });
    document.getElementById('led1-inv').addEventListener("click", function(){
        loadXMLDoc('led1-inv');
    });
    document.getElementById('led2-inv').addEventListener("click", function(){
        loadXMLDoc('led2-inv');
    });
    document.getElementById('led3-inv').addEventListener("click", function(){
        loadXMLDoc('led3-inv');
    });

    document.getElementById('led0-on').addEventListener("click", function(){
        loadXMLDoc('led0-on');
    });
    document.getElementById('led1-on').addEventListener("click", function(){
        loadXMLDoc('led1-on');
    });
    document.getElementById('led2-on').addEventListener("click", function(){
        loadXMLDoc('led2-on');
    });
    document.getElementById('led3-on').addEventListener("click", function(){
        loadXMLDoc('led3-on');
    });

    document.getElementById('led0-off').addEventListener("click", function(){
        loadXMLDoc('led0-off');
    });
    document.getElementById('led1-off').addEventListener("click", function(){
        loadXMLDoc('led1-off');
    });
    document.getElementById('led2-off').addEventListener("click", function(){
        loadXMLDoc('led2-off');
    });
    document.getElementById('led3-off').addEventListener("click", function(){
        loadXMLDoc('led3-off');
    });

    document.getElementById('adc0').addEventListener("click", function(){
        loadXMLDoc('adc0');
    });
    document.getElementById('adc1').addEventListener("click", function(){
        loadXMLDoc('adc1');
    });
    document.getElementById('adc2').addEventListener("click", function(){
        loadXMLDoc('adc2');
    });
    document.getElementById('adc3').addEventListener("click", function(){
        loadXMLDoc('adc3');
    });
}
