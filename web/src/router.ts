// UTILS
import log from "./utils/log";
import delay from "./utils/delay";
// ROUTING
import express, { response } from "express";
import cors from 'cors';
// MODULES
import fs from  'fs';
import * as child from 'child_process';
import { nextTick } from "process";

// ROUTING STUFF
let router: express.Router = express.Router();
router.use(cors())
router.use(express.json())
router.use(express.urlencoded({ extended: false }))

// WELCOMING REQUEST
router.use((req: express.Request, res: express.Response, next: express.NextFunction) => {
    let info: string = `${req.method} ${req.originalUrl}\n\tFROM: ${req.ip} | TIME: ${new Date().toISOString()}\n\tWPID: ${process.pid}`;
    if (req.headers)
        for (const prop in req.headers)
            info += `\n\tHEADER: ${prop}:${req.headers[prop]}`;
    if (req.query) info += "\n\tQUERY: " + JSON.stringify(req.query);
    if (req.body) info += "\n\tBODY: " + JSON.stringify(req.body);
    log("worker", info);
    next();
});

// TEST
router.get("/test", async (req: express.Request, res: express.Response) => {
    log("worker", "beginning testing", "d");
    await delay(100);
    log("worker", "finishing testing", "d");
    res.status(200).send('OK');
});

// SECONDARY ROUTES
/*import info_pl from "./routes/info-pl";
import info from "./routes/info";
import track from "./routes/track";
router.use(info_pl);
router.use(info);
router.use(track);*/

function makeid(length: number) {
    let result = '';
    let characters = 'abcdefghijklmnopqrstuvwxyz0123456789';
    let charactersLength = characters.length;
    for (let i = 0; i < length; i++) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    return result;
}

function receive(dir: string, req: express.Request, res: express.Response) {
    return new Promise<string>((resolve, reject) => {
        
        let file = `${makeid(12)}.tmp`;
        let path = `${dir}/${file}`;
        let data = Buffer.alloc(0);
        let delta = 5 * 60 * 1000;
        fs.readdirSync(dir).forEach((f) => {
            let filename = `${dir}/${f}`;
            console.log(filename);
            console.log(fs.statSync(filename).birthtimeMs);
            if (Date.now() + delta > fs.statSync(filename).birthtimeMs) {
                console.log(`${filename} -  ${Date.now() + delta} - ${fs.statSync(filename).birthtimeMs}`);
                try {
                    fs.rmSync(filename);
                } catch (e) {
                    console.log(e)
                }
            }
        });
        try {
            req.on('data', (chunk) => { data = Buffer.concat([data, chunk]); });
            req.on('end', async () => {
                if (!fs.existsSync(dir)) fs.mkdirSync(dir);                
                fs.writeFileSync(path, data, "binary");
                resolve(file);
            });
        } catch {
            reject();
        }
    });
}

router.use("", (req: express.Request, res: express.Response, next) => {
    log("worker", "debug", "d");
    next();
});

router.post("/compress", async (req: express.Request, res: express.Response) => {
    log("worker", "compress (POST)", "d");
    let dir = process.env.TEMP_DIR as unknown as string;
    let bin = process.env.BIN as unknown as string;
    let thr = req.query["thr"] ? req.query["thr"] as unknown as string : "63";
    try {
        let input = await receive(dir, req, res);
        let path = `${dir}/${input}`;
        let proc = child.execFileSync(bin, ["-c", "-f", path, "-t", thr]);
        let output = `${input}.dci`;
        let result = {
            fn: output,
            cr: child.execFileSync("./get-cr.sh", [`${dir}/${output}`, path]).toString()
        }
        res.status(200).send(result);
    } catch(e) {
        log("worker", `error\n\t${e}`, "e");
        res.status(500).send("error");
    }
});

router.post("/decompress", async (req: express.Request, res: express.Response) => {
    log("worker", "decompress (POST)", "d");
    let dir = process.env.TEMP_DIR as unknown as string;
    let bin = process.env.BIN as unknown as string;
    try {
        let filename = await receive(dir, req, res);
        let path = `${dir}/${filename}`;
        let proc = child.execFileSync(bin, ["-d", "-f", `${path}`]);
        let result = `${filename}.png`
        res.status(200).send(result);
    } catch(e) {
        log("worker", `error\n\t${e}`, "e");
        res.status(500).send("error");
    }
});

router.get("/retrieve", async (req: express.Request, res: express.Response) => {
    log("worker", "retrieve (GET)", "d");
    let dir = process.env.TEMP_DIR as unknown as string;
    let filename = req.query["fn"] ? req.query["fn"] as unknown as string : "63";
    try {
        let path = `${dir}/${filename}`;
        res.status(200).sendFile(path, {root: "/"});
        log("worker", `sending ${path}`, "d");
    } catch {
        res.status(500).send("something");
    }
});

router.use("", express.static("public"));

export default router;