const ctx = document.getElementById("trafficChart").getContext("2d");
const chart = new Chart(ctx, {
  type: "line",
  data: {
    labels: [],
    datasets: [{
      label: "Packets",
      data: [],
      fill: false,
      borderColor: "blue"
    }]
  }
});

const ws = new WebSocket("ws://localhost:8080");
ws.onmessage = (event) => {
  const now = new Date().toLocaleTimeString();
  chart.data.labels.push(now);
  chart.data.datasets[0].data.push(Math.random() * 10); // simulate value
  if (chart.data.labels.length > 20) {
    chart.data.labels.shift();
    chart.data.datasets[0].data.shift();
  }
  chart.update();
};
