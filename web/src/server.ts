// IMPORTS
import express from "express";
import router from './router';
import cluster from "cluster";
import log from "./utils/log";
import http from 'http';
import os from 'os';
import * as dotenv from 'dotenv';

async function main() {
    // CLUSTER GENERATION AND MANAGEMENT
    if (cluster.isMaster) {
    
        // IF THIS PROCESS IS MASTER...
        dotenv.config();
        
        // SOME DATA TO CALC 'N SHOW
        let date: Date = new Date();
        let workers_count: number = (process.env.WORKER_FROM_CORES == "true" ? os.cpus().length : process.env.WORKERS_DEFAULT as unknown as number);
        log("master", `time: ${date}`, "s");
        log("master", `pid: ${process.pid}`, "s");
        log("master", `worker count: ${workers_count}`, "s");
        log("master", `http://localhost:${process.env.PORT}${process.env.BASE_URL}`, "s");
    
        // EVENT HANDLING OF THE WORKER
        cluster.on('fork', (worker) => {
            log("master",`worker pid ${worker.process.pid} forked`, "s");
        });
        cluster.on('exit', (worker) => {
            log("master",`worker pid ${worker.process.pid} exited`, "s");
            cluster.fork();
        });
    
        // WORKER FORKING
        for (var i = 0; i < workers_count; i++)
            cluster.fork();
    
    } else if (cluster.isWorker) {
        
        // IF THIS PROCESS IS A WORKER...
        
        // VARIABLES
        const app: express.Application = express();

        // UNHANDLED EXCEPTIONS END UP HERE
        process.on('uncaughtException', (code: any, signal: any) => {
            log("WORKER",`worker uncaughtException\n\tcode:(${code})\n\tsignal:(${signal})`, "e");
            process.exit()
        })

        // CREATE AN APP, WHICH HANDLES REQ/RES WITH AN EXPRESS ROUTER
        app.use(process.env.BASE_URL as unknown as string, router as express.Router);
    
        let server: http.Server = http.createServer(app)
        server.listen(process.env.PORT)
    }
}

// ENTRY POINT
main();