var Socket = new WebSocket('ws://' + window.location.hostname + ':81')

Socket.onmessage = function (e) {
    var distancia = parseInt(e.data);
    console.log(distancia)
};