let queue = [];
//let base_url = "https://asky.hopto.org/dc";
let base_url = "http://localhost:2608/dc"; // `

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

function compress() {
    let number_thr = document.getElementById("number_thr");
    let file_compression = document.getElementById("file_compression");

    /*let data = new FormData();
    let reader = new FileReader();
    reader.onload = (evt) => {
        data.append("file", evt.target.result);
    }*/

    let xhr = new XMLHttpRequest();
    xhr.open("POST", `${base_url}/compress?thr=${number_thr.value}`, true);
    xhr.onload = (e) => {
        button.disabled = false;
        if (xhr.status == 200) {
            button.className = button.className.replace("btn-primary", "btn-success");
            button.innerHTML = "READY!";
            button.onclick = () => { download(index) }
        } else {
            button.innerHTML = "Download [.mp3]";
        }
    }
    xhr.onerror = (e) => {
        //aaaaa
    }

    //reader.readAsArrayBuffer(file_compression.files[0]);
    xhr.send(file_compression.files[0]);
}