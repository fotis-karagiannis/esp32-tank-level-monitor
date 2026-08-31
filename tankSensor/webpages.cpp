#include "webpages.h"

// Wi-Fi Setup Page
const char* wifiSetupPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<style>
*{box-sizing:border-box;}
body{font-family:Arial;background:#f4f4f9;padding:20px;}

.card{
    background:white;
    max-width:450px;
    margin:auto;
    padding:20px;
    border-radius:12px;
    box-shadow:0 4px 12px rgba(0,0,0,0.2);
    text-align:center;
}

.form-group.no-float{margin-bottom:25px;}
.static-label{
    margin-bottom:5px;
    font-size:1.1em;
    color:#444;
    display:block;
}

.form-group.no-float select{
    width:100%;
    padding:12px 10px;
    font-size:1em;
    border-radius:6px;
    border:1px solid #bbb;
}

.form-group{position:relative;margin-bottom:25px;}
label{
    position:absolute;
    top:12px;
    left:10px;
    color:#444;
    font-size:1em;
    transition:.2s;
}

input{
    width:100%;
    padding:12px 10px;
    font-size:1em;
    border-radius:6px;
    border:1px solid #bbb;
    background:none;
}

input:focus + label,
input:not(:placeholder-shown)+label{
    top:-8px;
    left:5px;
    font-size:0.9em;
    color:#0077ff;
    background:white;
    padding:0 3px;
}

.button-eye{cursor:pointer;}

button{
    width:100%;
    padding:18px;
    background:linear-gradient(90deg,#0077ff,#005fcc);
    color:white;
    font-size:1.2em;
    font-weight:bold;
    border:none;
    border-radius:8px;
    cursor:pointer;
}

button:hover{
    background:linear-gradient(90deg,#005fcc,#003f99);
}
</style>

<script>
function togglePassword(){
    let p=document.getElementById('pass'); 
    p.show=!p.show;
    p.type=p.show?'text':'password';
}

function validateForm(){
    return document.forms[0]['ssid'].value!='' &&
           document.forms[0]['pass'].value!='';
}

function hideSSIDLabel(){
    document.getElementById('ssidLabel').style.display='none';
}
</script>

<script>
// Hide SSID label if a network is already selected (browser cache/autofill)
window.addEventListener('DOMContentLoaded', () => {
    const sel = document.querySelector("select[name='ssid']");
    if (sel && sel.value && sel.value.trim() !== "") {
        document.getElementById('ssidLabel').style.display = 'none';
    }
});
</script>

</head>

<body>
<div class='card'>
<h2 style='font-size:1.5em;'>Tank Sensor Wi‑Fi Setup</h2>

<form action='/save' method='POST' onsubmit='return validateForm();'>

<div class='form-group no-float'>
<label class='static-label' id='ssidLabel'>Wi‑Fi Network</label>
<select name='ssid' onchange='hideSSIDLabel()'>%OPTIONS%</select>
</div>

<div class='form-group'>
<input id='pass' type='password' name='pass' placeholder=' ' required>
<label>Password</label>
<span onclick='togglePassword()' class='button-eye'
style='position:absolute;right:10px;top:50%;transform:translateY(-50%);font-size:1em;'>👁</span>
</div>

<button type='submit'>Save Wi‑Fi</button>

</form>
</div>
</body>
</html>
)rawliteral";


// Saved Page (with redirect)
const char* wifiSavedPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<style>
body{
    font-family:Arial;
    background:#f4f4f9;
    margin:0;
    padding:20px;
    display:flex;
    justify-content:center;
    align-items:center;
    height:100vh;
}

.card{
    background:white;
    max-width:450px;
    padding:30px;
    border-radius:12px;
    text-align:center;
    box-shadow:0 4px 12px rgba(0,0,0,0.2);
}

h2{
    color:#28a745;
    font-size:1.5em;
    margin-bottom:20px;
}

.loader{
    margin:20px auto;
    border:8px solid #eee;
    border-top:8px solid #0077ff;
    border-radius:50%;
    width:60px;
    height:60px;
    animation:spin 1s linear infinite;
}

@keyframes spin{
    0%{transform:rotate(0deg);}
    100%{transform:rotate(360deg);}
}
</style>

<script>
setTimeout(function(){
    fetch("http://tanksensor.local/", {mode:"no-cors"})
    .then(() => { window.location = "http://tanksensor.local/"; })
    .catch(() => { window.location = "http://" + window.location.hostname + "/"; });
}, 1500);
</script>

</head>
<body>
<div class='card'>
<h2>Settings Saved</h2>
<div class='loader'></div>
</div>
</body>
</html>
)rawliteral";


// Dimensions Setting Page
const char* dimensionsPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<style>
body{
    font-family:Arial;
    background:#f4f4f9;
    margin:0;
    padding:20px;
}

.card{
    background:white;
    max-width:450px;
    margin:auto;
    padding:20px;
    border-radius:12px;
    box-shadow:0 4px 12px rgba(0,0,0,0.2);
    text-align:center;
}

h2{
    font-size:1.3em;
    color:#333;
    margin-bottom:15px;
}

label{
    display:block;
    margin-top:12px;
    color:#444;
    font-size:1em;
    text-align:center;
}

.input-wrap{
    position:relative;
    width:85%;
    max-width:300px;
    margin:6px auto 0 auto;
}

.input-wrap input{
    width:100%;
    padding:10px 34px 10px 10px;
    border-radius:6px;
    border:1px solid #bbb;
    font-size:1em;
    box-sizing:border-box;
}

.input-wrap .unit{
    position:absolute;
    right:8px;
    top:50%;
    transform:translateY(-50%);
    color:#aaa;
    font-size:0.9em;
    pointer-events:none;
}

button{
    margin-top:20px;
    padding:14px 20px;
    font-size:1.1em;
    border:none;
    border-radius:10px;
    background:linear-gradient(90deg,#0077ff,#005fcc);
    color:white;
    cursor:pointer;
    width:100%;
}

.note{
    text-align:center;
    font-size:0.95em;
    color:#666;
    margin-top:12px;
}
</style>

<script>
function validate(){
  let w=document.forms[0]['width'].value;
  let l=document.forms[0]['length'].value;
  let h=document.forms[0]['height'].value;
  let o=document.forms[0]['offset'].value;
  return w>0 && l>0 && h>0 && o>=0;
}
</script>
</head>

<body>
<div class='card'>
<h2>Tank Dimensions (Rectangular Tank)</h2>

<form action='/saveDimensions' method='POST' onsubmit='return validate();'>

<label>Tank Width</label>
<div class='input-wrap'>
  <input name='width' type='number' step='0.1' min='0' value='%WIDTH%' required>
  <span class='unit'>cm</span>
</div>

<label>Tank Length</label>
<div class='input-wrap'>
  <input name='length' type='number' step='0.1' min='0' value='%LENGTH%' required>
  <span class='unit'>cm</span>
</div>

<label>Tank Height</label>
<div class='input-wrap'>
  <input name='height' type='number' step='0.1' min='0' value='%HEIGHT%' required>
  <span class='unit'>cm</span>
</div>

<label>Sensor Offset</label>
<div class='input-wrap'>
  <input name='offset' type='number' step='0.1' min='0' value='%OFFSET%' required>
  <span class='unit'>cm</span>
</div>

<button type='submit'>Save Dimensions</button>

<div class='note'>
All values must be in <strong>cm</strong>. Offset is the distance of the sensor from the top of the tank.
</div>

</form>
</div>
</body>
</html>
)rawliteral";


// Dashboard main page
const char* dashboardPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset='UTF-8'>
<meta name='viewport' content='width=device-width, initial-scale=1.0'>
<style>
body{
    font-family:Arial;
    background:#f4f4f9;
    margin:0;
    padding:20px;
}

.card{
    background:white;
    max-width:450px;
    margin:auto;
    padding:20px;
    border-radius:12px;
    box-shadow:0 4px 12px rgba(0,0,0,0.2);
    text-align:center;
}

h2{
    font-size:1.5em;
    color:#333;
    margin-bottom:15px;
}

.value{
    font-size:1.2em;
    color:#0077ff;
    margin:10px 0 16px 0;
}

small{
    display:block;
    color:#666;
    margin-top:6px;
    margin-bottom:14px;
}

button{
    width:100%;
    padding:14px 0;
    font-size:1.1em;
    border:none;
    border-radius:10px;
    background:linear-gradient(90deg,#0077ff,#005fcc);
    color:white;
    cursor:pointer;
    margin-top:12px;
}

button:hover{
    background:linear-gradient(90deg,#005fcc,#003f99);
}

.tank-svg{
    width:120px;
    height:240px;
    margin:20px auto 20px auto;
    display:block;
}

#tankFill{
    transition:height 0.6s ease, y 0.6s ease;
}
</style>

<script>
function refreshData(){
    fetch('/data').then(r=>r.json()).then(o=>{
        let value="";
        if(o.text=="NO_MEASUREMENT") value="No measurement yet";
        else if(o.text=="INVALID") value="Invalid Measurement";
        else value=o.text + (o.liters ? " — " + o.liters : "");
        document.getElementById('value').innerHTML=value;

        let note="";
        if(!o.tankConfigured) note="Tank dimensions not set. Click Tank Dimensions.";
        else note="Width: "+o.width+" cm; Length: "+o.length+" cm; Height: "+o.height+" cm; Offset: "+o.offset+" cm";
        document.getElementById('note').innerHTML=note;

        if(o.tankConfigured && o.liters_num>=0 && o.capacity_liters>0){
            let pct = Math.max(0, Math.min(100, (o.liters_num/o.capacity_liters)*100));
            let h = Math.round(192*pct/100);
            let y = 28 + (192-h);
            document.getElementById('tankFill').setAttribute('height', h);
            document.getElementById('tankFill').setAttribute('y', y);
            document.getElementById('capText').innerHTML = o.capacity_liters.toFixed(1)+" L";
        } else {
            document.getElementById('tankFill').setAttribute('height', 0);
            document.getElementById('tankFill').setAttribute('y', 220);
            document.getElementById('capText').innerHTML = "";
        }
    });
}

function takeMeasurement(){ fetch('/measure',{method:'POST'}).then(()=>setTimeout(refreshData,500)); }
function editDimensions(){ window.location='/dimensions'; }

setInterval(refreshData,1000);
window.onload=refreshData;
</script>
</head>

<body>
<div class='card'>
<h2>Fuel Level Calculator</h2>

<div id='value' class='value'>No measurement yet</div>
<small id='note'>Dimensions unknown</small>

<svg class='tank-svg' viewBox='0 0 120 240' xmlns='http://www.w3.org/2000/svg'>
  <rect x='10' y='2' width='100' height='18' fill='white' rx='4' ry='4'/>
  <text id='capText' x='60' y='14' font-size='12' text-anchor='middle' fill='#333'></text>

  <rect x='10' y='28' width='100' height='192'
        fill='#eee' stroke='#999' stroke-width='2' rx='6' ry='6'/>

  <rect id='tankFill' x='10' y='220' width='100' height='0'
        fill='#E6B400' rx='6' ry='6'/>
</svg>

<button onclick='takeMeasurement()'>Measure Fuel</button>

<button onclick='editDimensions()'
style='background:linear-gradient(90deg,#28a745,#1e7e34);'>
Tank Dimensions
</button>

</div>
</body>
</html>
)rawliteral";

