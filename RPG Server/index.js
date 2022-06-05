const express = require('express');
const app = express();
const http = require('http');
const server = http.createServer(app);
const { Server } = require("socket.io");
const io = new Server(server);

let users = [];
app.get('/', (req, res) => {
    res.send("Hey");
});
setInterval(() => {
    io.emit("game_tick", users);
}, 30);
io.on('connection', (socket) => {

    console.log(socket.id, 'connected');

    socket.on('send_inital_user_data', (posX, posY) => {
        socket.broadcast.emit("user_joined", { user: socket.id, x: posX, y: posY })
        const getUser = users.findIndex(({ user }) => user == socket.id);
        users[getUser].x = posX;
        users[getUser].y = posY;
    })

    socket.on('user_update', (posX, posY) => {
        //socket.broadcast.emit("user_joined", { user: socket.id, x: posX, y: posY})
        const getUser = users.findIndex(({ user }) => user == socket.id);
        users[getUser].x = posX;
        users[getUser].y = posY;
    })

    socket.on('user_wants_connection', (data) => {
        users.push({ user: socket.id, x: "0", y: "0" })
        socket.emit("user_got_connected", { user: socket.id })

    })
    socket.on('disconnect', () => {
        console.log(socket.id, 'disconnected')
        users.splice(users.findIndex(({ user }) => user == socket.id), 1);
    })
});

setInterval(() => { io.emit("cyka"); console.log(users) }, 1000);
server.listen(4545, () => {
    console.log('listening on *:4545');
});