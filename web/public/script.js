let queue = [];
let base_url = "https://asky.hopto.org/dc";
//let base_url = "http://localhost:2608/dc"; // `

function update_thr(value) {
    let number_thr = document.getElementById("number_thr");
    let range_thr = document.getElementById("range_thr");
    if (value < 0) {
        value = 0;
    } else if (value > 255) {
        value = 255;
    }
    number_thr.value = value;
    range_thr.value = value;
}

function update_button_compression() {
    let file_compression = document.getElementById("file_compression");
    let number_thr = document.getElementById("number_thr");
    let button_compression = document.getElementById("button_compression");
    button_compression.disabled = !(file_compression.value && number_thr.value >= 0 && number_thr.value <= 255);
}

function update_button_decompression() {
    let file_decompression = document.getElementById("file_decompression");
    let button_decompression = document.getElementById("button_decompression");
    button_decompression.disabled = !(file_decompression.value);
}

function download(filename) {
    let url = new URL(`${base_url}/retrieve?fn=${filename}`);
    var element = document.createElement('a');
    element.style.display = 'none';
    element.href = url;
    element.download = filename;
    element.target="_blank"
    document.body.appendChild(element);
    element.click();
    document.body.removeChild(element);
}


function compress() {
    let number_thr = document.getElementById("number_thr");
    let file_compression = document.getElementById("file_compression");
    let button = document.getElementById("button_compression");
    let label = document.getElementById("label_cr");
    button.disabled = true;
    let xhr = new XMLHttpRequest();
    xhr.open("POST", `${base_url}/compress?thr=${number_thr.value}`, true);
    xhr.onload = (e) => {
        if (xhr.status == 200) {
            let res = JSON.parse(xhr.response);
            label.innerHTML = res.cr;
            download(res.fn);
        } else {
            alert("Error!\n" + xhr.response);
        }
        button.disabled = false;
    }
    xhr.onerror = (e) => {
        button.disabled = false;
    }
    xhr.send(file_compression.files[0]);
}

function decompress() {
    let file_decompression = document.getElementById("file_decompression");
    let button = document.getElementById("button_decompression")
    button.disabled = true;
    let xhr = new XMLHttpRequest();
    xhr.open("POST", `${base_url}/decompress`, true);
    xhr.onload = (e) => {
        if (xhr.status == 200) {
            download(xhr.response);
        } else {
            alert("Error!\n" + xhr.response);
        }
        button.disabled = false;
    }
    xhr.onerror = (e) => {
        button.disabled = false;
    }
    xhr.send(file_decompression.files[0]);
}