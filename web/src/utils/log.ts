import color from 'colorts';
import fs from 'fs';

function painter(h: string, f: string) {
    let res: any;
    switch (f) {
        case "e": res = color(h).red; break // ERROR
        case "s": res = color(h).green; break // SUCCESS
        case "b": res = color(h).gray; break // BACKGROUND
        case "d": res = color(h).magenta; break // DEBUG
        case "w": res = color(h).yellow; break // WARNING
        case "n": res = color(h).cyan; break // NORMAL
        default: res = color(h).cyan
    }
    return res;
}   

export default function log(h: string, s: string, f: string ="n") {
    console.log(`${painter(`[${h.toUpperCase()}]`, f)} ${s}`)
    //fs.appendFile('log.txt', `[${h.toUpperCase()}] ${s}\n`, (err) => {})
}