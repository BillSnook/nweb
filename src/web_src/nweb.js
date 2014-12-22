//  ----    ----
//	nweb.js
//	
//  ----    ----


function getOurXMLHttpRequest() {
	
    var newhttp = false;
	try {
        newhttp = new XMLHttpRequest();
    } catch (E) {
        try {
            newhttp = new ActiveXObject( "Msxml2.XMLHTTP" );
        } catch (E) {
            try {
                newhttp = new ActiveXObject( "Microsoft.XMLHTTP" );
            } catch (E) {
                return false;
            }
        }
    }
	return newhttp;
}


//  ----    ----


function loadDivObjWithPage( destDiv, dataPage ) {

//	alert( "loadDivObjWithPage" );
    var divhttp = getOurXMLHttpRequest();
	if ( divhttp ) {
		divhttp.open( "GET", dataPage );
		divhttp.onreadystatechange = function() {
			if ( divhttp.readyState == 4 ) {
				// Non-html data sent from our nweb server is without headers so no status is delivered
				if ( ( 200 == divhttp.status ) || ( 0 == divhttp.status ) ) {
//					alert("In loadDivObjWithPage, got response " + divhttp.responseText);
					destDiv.innerHTML = divhttp.responseText;
				} else {
					alert("In loadDivObjWithPage, status returned: " + divhttp.status + ", status text: " + divhttp.statusText + ", response text: " + divhttp.responseText );
				}
			}
		}
		divhttp.send(null);
	} else {
		alert( "In loadDivObjWithPage, unable to get XMLHttpRequest" );
		return false;
	}
    return true;
}


//  ----    ----


function setup_clock() {
	
//	alert( "In setup_clock" );
	show_clock();
	window.setInterval( "show_clock()", 1000 );
// Milliseconds - nominally 1 second for clock update
}


function show_clock() {
//	alert( "In show_clock" );
//	if ( !document.layers && !document.all && !document.getElementById )
//		return;
	
	var dayName = new Array ( "Sun", "Mon", "Tue", "Wed", "Thurs", "Fri", "Sat" );
//	var dayName = new Array ( "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" );
	var monName = new Array ( "Jan.", "Feb.", "Mar.", "Apr.", "May", "June", "July", "Aug.", "Sept.", "Oct.", "Nov.", "Dec." );
	
	var timeNow = new Date();
	
	var day = timeNow.getDay();			// Day of week
	var month = timeNow.getMonth();
	var date = timeNow.getDate();		// Date of day in month
	var hours = timeNow.getHours();
	var minutes = timeNow.getMinutes();
	var seconds = timeNow.getSeconds();

	var dn = "PM";
	if ( hours < 12 )
		dn = "AM";
	if ( hours > 12 )
		hours = hours - 12;
	if ( hours == 0 )
		hours = 12;
					
	if ( minutes <= 9 )
		minutes = "0" + minutes;
	if ( seconds <= 9 )
		seconds = "0" + seconds;
	//change font size here to your desire
	var myclock = "<b>" + dayName[ day ] + "&nbsp;&nbsp;&nbsp;" + monName[ month ] + " " + date + "&nbsp;&nbsp;&nbsp;";
	myclock += hours + ":" + minutes + ":" + seconds + "&nbsp;&nbsp;" + dn + "</b>";
//	if ( document.layers ) {
//		document.layers.liveclock.document.write( myclock );
//		document.layers.liveclock.document.close();
//	} else
//	if ( document.all )
		liveclock.innerHTML = myclock;
//	else
//	if ( document.getElementById )
//		document.getElementById( "liveclock").innerHTML = myclock;
}


//  ----    ----

var monitorhttp = getOurXMLHttpRequest();
var monitor

function setup_monitor() {
	
//	alert( "In setup_monitor, got " + bannerPane );
	show_clock();
	window.setInterval( "show_monitor()", 1000 );	// Milliseconds - nominally 1 second for clock update
}


function show_monitor() {
		
}

//  ----    ----


// End of nweb.js
