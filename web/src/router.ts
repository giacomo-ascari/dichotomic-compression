// UTILS
import log from "./utils/log";
import delay from "./utils/delay";
// ROUTING
import express, { response } from "express";
import cors from 'cors';
// MODULES
import fs from  'fs';

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

router.use("", express.static("public"));

export default router;