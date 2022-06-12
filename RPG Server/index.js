import Player from './player.class.js';
import Queue from './queue.class.js';
import express from 'express';
const app = express();
import http from 'http';
const server = http.createServer(app);
import { Server } from 'socket.io';
const io = new Server(server);
let delta;
let ptime = Date.now();    // Time since last frame
let offset = 0;
let tickRate = 1000 / 60;
let updateQueue = new Queue();

const deltaTime = (previous, offset) => {
    let Δt = (Date.now() - previous);  // Δt = Current time - time of last frame
    return Δt;
}

const delay = async (time) => {
    return new Promise((resolve) => {
        setTimeout(() => { resolve() }, time)
    })
}

let users = [];
app.get('/', (req, res) => {
    res.send("Hello");
});

/* 
// Game tick loop
*/
setInterval(() => {

    const userChunk = []
    users.forEach(user => {
        userChunk.push({ socketid: user.socketid, x: user.x, y: user.y })
    })

    console.log(userChunk)
    io.emit("game_tick", userChunk);
}, 45);

/*
// Physics tick loop
*/
setInterval(() => {
    delta = deltaTime(ptime, offset);
    ptime = Date.now();
    offset = delta % (1000 / tickRate);
    ptime = Date.now();

    io.emit("physics_update_tick")

    //if (updateQueue.isEmpty()) return;
    users.forEach((user, index) => {
        //console.log(`Player ${index + 1}'s`, 'X:', user.x);
        //console.log(`Player ${index + 1}'s`, 'Y:', user.y);
        const getActions = updateQueue.items.filter(item => item.user === user.socketid)
        //const getUser = users.find(user => user.socketid == "CPU")
        if (!getActions) return;
        //console.log(getActions)
        getActions.forEach((action, index) => {
            if (action.action.move) {
                //console.log(action)
                if (action.action.move.dir === "right") {
                    user.x += (1 * 0.3) * action.action.delta
                    updateQueue.dequeue(index)
                }
                if (action.action.move.dir === "left") {
                    user.x -= (1 * 0.3) * action.action.delta
                    updateQueue.dequeue(index)
                }
                if (action.action.move.dir === "up") {
                    user.y -= (1 * 0.3) * action.action.delta
                    updateQueue.dequeue(index)
                }
                if (action.action.move.dir === "down") {
                    user.y += (1 * 0.3) * action.action.delta
                    updateQueue.dequeue(index)
                }
            }
        })
    })
}, 15)
const CPU = new Player('', 'CPU')
users.push(CPU);

setInterval(() => {
    const getCPU = users.find(user => user.socketid === "CPU");
    getCPU.x = 0;
    getCPU.y = 0;
}, 3000)
setInterval(() => {
    updateQueue.enqueue({ user: "CPU", action: { move: { dir: "right" }, delta: 15 } });
    updateQueue.enqueue({ user: "CPU", action: { move: { dir: "down" }, delta: 15 } });
    //updateQueue.enqueue({ user: "CPU", action: { move: { dir: "left" }, delta: 15 } });
    //updateQueue.enqueue({ user: "CPU", action: { move: { dir: "up" }, delta: 15 } });
}, 45)

io.on('connection', (socket) => {

    const player = new Player('', socket.id)
    users.push(player);

    socket.onAny((event, data) => {
        //console.log(event, Buffer.from(data).toString())
    })

    socket.on('send_inital_user_data', (username, posX, posY) => {
        socket.broadcast.emit("user_joined", { user: socket.id, username: username, x: posX, y: posY })
        const getUser = users.findIndex(({ user }) => user == socket.id);
        users[getUser].username = username
        users[getUser].x = posX;
        users[getUser].y = posY;
    })

    socket.on('move', (dir, delta, previousPos, time) => {
        //console.log(new Date(time*1000))
        const direction = Buffer.from(dir).toString()
        if (direction === 'u') {
            player.up_pressed = true;
            updateQueue.enqueue({ user: socket.id, action: { move: { dir: "up" }, delta: delta } });
        }
        if (direction === 'd') {
            player.down_pressed = true;
            updateQueue.enqueue({ user: socket.id, action: { move: { dir: "down" }, delta: delta } });
        }
        if (direction === 'l') {
            player.left_pressed = true;
            updateQueue.enqueue({ user: socket.id, action: { move: { dir: "left" }, delta: delta } });
        }
        if (direction === 'r') {
            player.right_pressed = true;
            updateQueue.enqueue({ user: socket.id, action: { move: { dir: "right" }, delta: delta } });
        }
    })

    //socket.emit('force_pos', {x:player.x, y:player.y});
    socket.on('user_update', (username, posX, posY) => {
        //socket.broadcast.emit("user_joined", { user: socket.id, x: posX, y: posY})
        const getUser = users.findIndex(({ user }) => user == socket.id);
        users[getUser].username = username
        users[getUser].x = posX;
        users[getUser].y = posY;
    })

    socket.on('user_wants_connection', (username) => {
        //users.push({ user: socket.id, x: "0", y: "0", username: username })
        socket.emit("user_got_connected", { user: socket.id })

    })
    socket.on('disconnect', () => {
        console.log(socket.id, 'disconnected')
        users.splice(users.findIndex(({ user }) => user == socket.id), 1);
    })
});

let running = true;
// while(running){
//   delta = deltaTime(ptime, offset);
//   ptime = Date.now();
//   offset = delta % (1000 / tickRate);
//   ptime = Date.now();
//   console.log(delta)
//   await delay(tickRate)
// }
server.listen(4545, () => {
    console.log('listening on *:3000');
});