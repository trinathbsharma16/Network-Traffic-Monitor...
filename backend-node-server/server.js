const dgram = require("dgram");
const WebSocket = require("ws");

const udpServer = dgram.createSocket("udp4");
const wss = new WebSocket.Server({ port: 8080 });

let clients = [];

wss.on("connection", (ws) => {
  clients.push(ws);
  ws.on("close", () => {
    clients = clients.filter(client => client !== ws);
  });
});

udpServer.on("message", (msg) => {
  const data = msg.toString();
  clients.forEach(ws => {
    if (ws.readyState === WebSocket.OPEN) {
      ws.send(data);
    }
  });
});

udpServer.bind(8081, () => {
  console.log("UDP Server listening on port 8081");
});
