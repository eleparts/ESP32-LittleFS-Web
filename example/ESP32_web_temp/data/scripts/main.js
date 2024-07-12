if (!!window.EventSource) {
    var source = new EventSource('/events');

    source.addEventListener('open', function (e) {
        console.log("Events Connected");
    }, false);

    source.addEventListener('error', function(e) {
        if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
        }
    }, false);


    source.addEventListener('myeventTemp', function (e) {
        var data = e.data;
        document.getElementById("temp").innerHTML = data;
    }, false);

    source.addEventListener('myeventHumid', function (e) {
        var data = e.data;
        document.getElementById("humid").innerHTML = data;
    }, false);
}