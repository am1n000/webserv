const http = require('http');
const { performance } = require('perf_hooks');

const url = 'http://localhost:8080/index.html';
const numRequests = 2;

const start = performance.now();
let numSuccesses = 0;

for (let i = 0; i < numRequests; i++) {
  http.get(url, (res) => {
    // console.log("requets sent");
    if (res.statusCode === 200) {
      numSuccesses++;
    }
    console.log(`sent ${numSuccesses} requests`);
    if (numSuccesses === numRequests) {
      const end = performance.now();
      const totalTime = (end - start) / 1000;
      const requestsPerSecond = numRequests / totalTime;
      console.log(`Total requests: ${numRequests}`);
      console.log(`Total time: ${totalTime.toFixed(2)} seconds`);
      console.log(`Requests per second: ${requestsPerSecond.toFixed(2)}`);
    }
  });
}
