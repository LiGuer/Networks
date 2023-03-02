function logIn() {
    $(".signUp").remove();
    $(".logIn").remove();

    t = "\
        <div class = 'logIn'><br>\
        <form action = 'http://192.168.222.128:9999' method = 'GET'>\
            <input class = 'input' type = 'text' value = 'username' name = 'username' onclick = \"if(value == defaultValue) {value = ''; this.style.color = '#000';}\"></input>\
            <br>\
            <input class = 'input'  type = 'text' value = 'password' name = 'password' onclick = \"if(value == defaultValue) {value = ''; this.style.color = '#000';}\"></input>\
            <br><br>\
            <button class='button submit' onmousedown='submit()'>Submit</button>\
        </form>\
        </div>";
    $(".MainBox").append(t);
}

function signUp() {
    $(".signUp").remove();
    $(".logIn").remove();

    t = "\
        <div class = 'signUp'><br>\
        <form action = 'https://www.baidu.com' method = 'GET'>\
            <input class = 'input' type = 'text' value = 'username' name = 'username' onclick = \"if(value == defaultValue) {value = ''; this.style.color = '#000';}\"></input>\
            <br>\
            <input class = 'input'  type = 'text' value = 'password' name = 'password' onclick = \"if(value == defaultValue) {value = ''; this.style.color = '#000';}\"></input>\
            <br><br>\
            <button class='button submit' onmousedown='submit()'>Submit</button>\
        </form>\
        </div>";
    $(".MainBox").append(t);
}

function submit() {
    $(".signUp").remove();
    $(".logIn").remove();
}