/*
 * html.h
 *
 *  Created on: 03.11.2017
 *      Author: Xiaoqiao
 */

#ifndef HTML_H_
#define HTML_H_

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define RESPONSE_HEADER HTTP/1.1 200 OK\r\nContent-type:text/html;charset=utf-8\r\nAccess-Control-Allow-Origin: *\r\n\r\n

#define HTML_HEAD_TOP <html><head>\
	<script>\
		function execOperation(path, element) {\
			var xhttp = new XMLHttpRequest();\
			xhttp.onreadystatechange=function() {\
				if( this.responseText.length > 0){\
					element.outerHTML = this.responseText;\
					console.log(this.responseText);}};\
			xhttp.open("GET", path, true);\
			xhttp.send();\
		}\
		function GetOperation(path) {\
			var xhttp = new XMLHttpRequest();\
			xhttp.onreadystatechange=function() {\
				if (this.readyState == 4 && this.status == 200) {\
					document.getElementById("innhalt").innerHTML =\
					this.responseText;\
					console.log(this.responseText);\
				}\
			};\
			xhttp.open("GET", "http://"+path+"/GET_OPERATIONS/", true);\
			xhttp.send();\
		}
#define HTML_HEAD_CENTER	function GlobalOP(path) {\
			var xhttp = new XMLHttpRequest();\
			xhttp.onreadystatechange=function() {};\
			xhttp.open("GET", "http://%s/Alle/"+path+"/", true);\
			xhttp.send();\
			xhttp = new XMLHttpRequest();\
			xhttp.onreadystatechange=function() {};\
			xhttp.open("GET", "http://%s/Alle/"+path+"/", true);\
			xhttp.send();\
		}\
	</script>
#define HTML_HEAD_BOTTOM 	<style> button {} \
		#innhalt{ position: relative; width : 24em;font-size: 2.5em;text-align:center;}\
		div.tab button {\
			font-size: 2.5em; position: relative; width:12em;height:3em; \
			float: left;\
			outline: none;\
			cursor: pointer;\
			padding: 14px 16px;\
			transition: 0.3s;\
		}\
		.t1{\
			background-color:  #008CBA;\
			border: 2px solid #008CBA;\
		}\
		div.tab {\
			width : 60em;\
			overflow: hidden;\
			border: 1px solid #ccc;\
			background-color: #f1f1f1;\
			margin-bottom:1em;\
		}\
		div.tab button:hover {\
			background-color: #ddd;\
		}\
		div.tab button.active {\
			background-color: #ccc;\
		}\
	</style>\
	</head>\
		<body>
#define HTML_BODY <div class="tab">\
				<button class="t1" onclick="GetOperation('%s')">EZ Jayde AZ</button>\
				<button class="t2" onclick="GetOperation('%s')">WZ Gang</button>\
				<button class="t2" onclick="GlobalOP('ganz_zu')">Rollos zu</button>\
				<button class="t1" onclick="GlobalOP('ganz_auf')">Rollos auf</button>\
				<button class="t1" onclick="GlobalOP('an')">Licht an</button>\
				<button class="t2" onclick="GlobalOP('aus')">Licht aus</button>\
			</div>\
			<div id = "innhalt">

#define HTML_INFO </div><div id = "info">

#define HTML_FOOTER </div>\
			<svg class="symbol" >\
				<symbol id="arrow_up" viewBox="00 00 700 700" >\
					<path d="M350,0 700,700 350,550 0,700" fill="#0C0"/>\
				</symbol>\
			</svg>\
			<svg class="symbol" >\
				<symbol id="arrow_full_up" viewBox="00 00 700 700" >\
					<path d="M350,0 700,700 350,550 0,700" fill="#0C0"/>\
					<path d="M350,0 700,500 350,350 0,500" fill="#0C0"/>\
				</symbol>\
			</svg>\
			<svg class="symbol" >\
				<symbol id="arrow_down" viewBox="00 00 700 700" >\
					<g transform="rotate(180 350 350)">\
						<path d="M350,0 700,700 350,550 0,700" fill="#ff0000"/>\
					</g>\
				</symbol>\
			</svg>\
			<svg class="symbol" >\
				<symbol id="arrow_full_down" viewBox="00 00 700 700" >\
					<g transform="rotate(180 350 350)">\
						<path d="M350,0 700,700 350,550 0,700" fill="#ff0000"/>\
						<path d="M350,0 700,500 350,350 0,500" fill="#ff0000"/>\
					</g>\
				</symbol>\
			</svg>\
			<svg class="symbol" >\
				<symbol id="ligth_on" viewBox="00 00 100 100" >\
					<g>\
						<path fill="#FFE75A" d="M49.958,28.688c-9.278,0-16.804,7.525-16.804,16.81c0,3.056,0.857,5.898,2.24,8.391\
					c4.126,7.438,5.221,11.613,5.896,15.654c0.564,3.387,1.344,4.174,3.902,4.174c1.091,0,2.869,0,4.766,0c1.901,0,3.683,0,4.767,0\
					c2.563,0,3.344-0.787,3.907-4.174c0.677-4.043,1.771-8.219,5.896-15.654c1.388-2.492,2.24-5.334,2.24-8.391\
					C66.771,36.212,59.245,2	8.688,49.958,28.688z"/>\
						<path fill="#FFE75A" d="M55.971,74.934H43.958c-0.5,0-0.903,0.402-0.903,0.9v0.602c0,0.496,0.403,0.9,0.903,0.9h12.013\
					c0.493,0,0.897-0.404,0.897-0.9v-0.602C56.87,75.336,56.464,74.934,55.971,74.934z"/>\
						<path fill="#FFE75A" d="M55.971,78.537H43.958c-0.5,0-0.903,0.4-0.903,0.9v0.602c0,0.496,0.403,0.898,0.903,0.898h12.013\
					c0.493,0,0.897-0.402,0.897-0.898v-0.602C56.87,78.938,56.464,78.537,55.971,78.537z"/>\
						<path fill="#FFE75A" d="M55.971,82.141H43.958c-0.5,0-0.903,0.402-0.903,0.9v0.602c0,0.498,0.403,0.9,0.903,0.9h12.013\
					c0.493,0,0.897-0.402,0.897-0.9v-0.602C56.87,82.543,56.464,82.141,55.971,82.141z"/>\
						<path fill="#FFE75A" d="M45.458,85.744h9.013c0,1.801-2.406,3.002-4.513,3.002C47.861,88.746,45.458,87.545,45.458,85.744z"/>\
						<path fill="#FFE75A" d="M69.264,16.854c1.532-2.683-2.605-5.092-4.15-2.414c-0.809,1.399-6.662,11.541-6.662,11.541l4.162,2.402\
					c0,0,5.199-9.017,6.646-11.518c0.003-0.001,0.003-0.003,0.003-0.003L69.264,16.854L69.264,16.854z"/>\
						<path fill="#FFE75A" d="M78.633,26.247L78.633,26.247L78.63,26.25c-0.008,0.001-0.008,0.001-0.008,0.001\
					C76.51,27.472,67.456,32.7,67.456,32.7l2.397,4.16c0,0,9.076-5.239,11.188-6.458C83.727,28.854,81.316,24.713,78.633,26.247z"/>\
						<path fill="#FFE75A" d="M52.452,11.071c0,0.729,0,12.812,0,12.812h-4.806c0,0,0-12.084,0-12.812\
					C47.646,7.973,52.452,7.973,52.452,11.071z"/>\
						<path fill="#FFE75A" d="M30.737,16.854c-1.532-2.683,2.606-5.092,4.152-2.414c0.808,1.399,6.661,11.541,6.661,11.541l-4.158,2.402\
					c0,0-5.208-9.017-6.652-11.518c0-0.001,0-0.003,0-0.003L30.737,16.854L30.737,16.854z"/>\
						<path fill="#FFE75A" d="M21.371,26.247L21.371,26.247v0.003c0.007,0.001,0.007,0.001,0.007,0.001\
					c2.116,1.221,11.167,6.449,11.167,6.449l-2.402,4.16c0,0-9.076-5.239-11.187-6.458C16.273,28.854,18.683,24.713,21.371,26.247z"/>\
					</g>\
				</symbol>   \
			</svg>\
			<svg class="symbol" >\
				<symbol id="ligth_off" viewBox="00 00 100 100" >\
					<g>\
						<path fill="#000000" d="M49.958,28.688c-9.278,0-16.804,7.525-16.804,16.81c0,3.056,0.857,5.898,2.24,8.391\
					c4.126,7.438,5.221,11.613,5.896,15.654c0.564,3.387,1.344,4.174,3.902,4.174c1.091,0,2.869,0,4.766,0c1.901,0,3.683,0,4.767,0\
					c2.563,0,3.344-0.787,3.907-4.174c0.677-4.043,1.771-8.219,5.896-15.654c1.388-2.492,2.24-5.334,2.24-8.391\
					C66.771,36.212,59.245,28.688,49.958,28.688z"/>\
						<path fill="#000000" d="M55.971,74.934H43.958c-0.5,0-0.903,0.402-0.903,0.9v0.602c0,0.496,0.403,0.9,0.903,0.9h12.013\
					c0.493,0,0.897-0.404,0.897-0.9v-0.602C56.87,75.336,56.464,74.934,55.971,74.934z"/>\
						<path fill="#000000" d="M55.971,78.537H43.958c-0.5,0-0.903,0.4-0.903,0.9v0.602c0,0.496,0.403,0.898,0.903,0.898h12.013\
					c0.493,0,0.897-0.402,0.897-0.898v-0.602C56.87,78.938,56.464,78.537,55.971,78.537z"/>\
						<path fill="#000000" d="M55.971,82.141H43.958c-0.5,0-0.903,0.402-0.903,0.9v0.602c0,0.498,0.403,0.9,0.903,0.9h12.013\
					c0.493,0,0.897-0.402,0.897-0.9v-0.602C56.87,82.543,56.464,82.141,55.971,82.141z"/>\
						<path fill="#000000" d="M45.458,85.744h9.013c0,1.801-2.406,3.002-4.513,3.002C47.861,88.746,45.458,87.545,45.458,85.744z"/>\
						<path fill="#000000" d="M69.264,16.854c1.532-2.683-2.605-5.092-4.15-2.414c-0.809,1.399-6.662,11.541-6.662,11.541l4.162,2.402\
					c0,0,5.199-9.017,6.646-11.518c0.003-0.001,0.003-0.003,0.003-0.003L69.264,16.854L69.264,16.854z"/>\
						<path fill="#000000" d="M78.633,26.247L78.633,26.247L78.63,26.25c-0.008,0.001-0.008,0.001-0.008,0.001\
					C76.51,27.472,67.456,32.7,67.456,32.7l2.397,4.16c0,0,9.076-5.239,11.188-6.458C83.727,28.854,81.316,24.713,78.633,26.247z"/>\
						<path fill="#000000" d="M52.452,11.071c0,0.729,0,12.812,0,12.812h-4.806c0,0,0-12.084,0-12.812\
					C47.646,7.973,52.452,7.973,52.452,11.071z"/>\
						<path fill="#000000" d="M30.737,16.854c-1.532-2.683,2.606-5.092,4.152-2.414c0.808,1.399,6.661,11.541,6.661,11.541l-4.158,2.402\
					c0,0-5.208-9.017-6.652-11.518c0-0.001,0-0.003,0-0.003L30.737,16.854L30.737,16.854z"/>\
						<path fill="#000000" d="M21.371,26.247L21.371,26.247v0.003c0.007,0.001,0.007,0.001,0.007,0.001\
					c2.116,1.221,11.167,6.449,11.167,6.449l-2.402,4.16c0,0-9.076-5.239-11.187-6.458C16.273,28.854,18.683,24.713,21.371,26.247z"/>\
					</g>\
				</symbol>   \
			</svg></body></html>

#define ARROW_LABEL <text x="60" y="100" font-size="50" fill="black">%c</text>
#define LIGHT_ON <svg width="150" height="150"><use xlink:href="#ligth_on" /></svg>
#define LIGHT_OFF <svg width="150" height="150"><use xlink:href="#ligth_off" /></svg>
#define ARROW_UP <svg width="150" height="150"><use xlink:href="#arrow_up" />ARROW_LABEL</svg>
#define ARROW_DOWN <svg width="150" height="150"><use xlink:href="#arrow_down" />ARROW_LABEL</svg>
#define ARROW_FULL_UP <svg width="150" height="150"><use xlink:href="#arrow_full_up" />ARROW_LABEL</svg>
#define ARROW_FULL_DOWN <svg width="150" height="150"><use xlink:href="#arrow_full_down" />ARROW_LABEL</svg>
#endif /* HTML_H_ */
