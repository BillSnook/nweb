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
				if ( divhttp.status == 200 ) {
//					alert("In loadDivObjWithPage, got response " + divhttp.responseText);
					destDiv.innerHTML = divhttp.responseText;
				} else {
					alert("In loadDivObjWithPage, status returned: " + divhttp.status + ", status text: " + divhttp.statusText );
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
	
//	alert( "In setup_clock, got " + bannerPane );
	show_clock();
	window.setInterval( "show_clock()", 1000 );
}


function show_clock() {
	if ( !document.layers && !document.all && !document.getElementById )
		return;
		
	var dayName = new Array ( "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" );
	var monName = new Array ( "Jan.", "Feb.", "Mar.", "Apr.", "May", "June", "July", "Aug.", "Sept.", "Oct.", "Nov.", "Dec." );
	
	var Digital = new Date();
	
	var day = Digital.getDay();
	var month = Digital.getMonth();
	var date = Digital.getDate();
	var hours = Digital.getHours();
	var minutes = Digital.getMinutes();
	var seconds = Digital.getSeconds();

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
	var myclock = dayName[ day ] + ", " + monName[ month ] + " " + date + ", ";
	myclock += hours + ":" + minutes + ":" + seconds + " " + dn + "</b>";
	if ( document.layers ) {
		document.layers.liveclock.document.write( myclock );
		document.layers.liveclock.document.close();
	} else
	if ( document.all )
		liveclock.innerHTML = myclock;
	else
	if ( document.getElementById )
		document.getElementById( "liveclock").innerHTML = myclock;
}

