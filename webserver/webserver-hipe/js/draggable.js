// Make the DIV element draggable:


function draggableAndresizable(elmnt) {
    $(elmnt).draggable().resizable();
}


function draggable(elmnt) {
    $(elmnt).draggable();
}

function dragElement2(elmnt) {
    var pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
    if (document.getElementById(elmnt.id + "header")) {
        // if present, the header is where you move the DIV from:
        document.getElementById(elmnt.id + "header").onmousedown = dragMouseDown;
    } else {
        // otherwise, move the DIV from anywhere inside the DIV: 
        elmnt.onmousedown = dragMouseDown;
    }

    function getXY(evt, element) {
        var rect = element.getBoundingClientRect();
        var scrollTop = document.documentElement.scrollTop?
            document.documentElement.scrollTop:document.body.scrollTop;
        var scrollLeft = document.documentElement.scrollLeft?                   
            document.documentElement.scrollLeft:document.body.scrollLeft;
        var elementLeft = rect.left+scrollLeft;  
        var elementTop = rect.top+scrollTop;
        elementTop = elementTop < 0 ? -elementTop : elementTop;
        elementLeft = elementLeft < 0 ? -elementLeft : elementLeft;
        x = evt.pageX-elementLeft;
        y = evt.pageY-elementTop;

        return {x:x, y:y};
    }

    function dragMouseDown(e) {
        e = e || window.event;
        e.preventDefault();
        // get the mouse cursor position at startup:
        pos3 = e.clientX;
        pos4 = e.clientY;
        document.onmouseup = closeDragElement;
        // call a function whenever the cursor moves:
        document.onmousemove = elementDrag;
    }

    function elementDrag(e) {
        e = e || window.event;
        e.preventDefault();
        // calculate the new cursor position:
        pos1 = pos3 - e.clientX;
        pos2 = pos4 - e.clientY;
        pos3 = e.clientX;
        pos4 = e.clientY;
        //var m = getXY(e, elmnt);
        //Resizing
        //if (m.x >= elmnt.offsetWidth - 20 && m.x < elmnt.offsetWidth && 
        //    m.y >= elmnt.offsetHeight - 20 & m.y < elmnt.offsetHeight ) {
        //    elmnt.style.width = m.x + 'px';
        //    elmnt.style.height = m.y + 'px';
        //    for (var el = 0; el < elmnt.childElementCount; el++) {
        //        elmnt.children.item(el).style.width = m.x + 'px';
        //        elmnt.children.item(el).style.height = m.x + 'px';
        //    }

        //} else 
        { // Move div
        
            // set the element's new position:
            elmnt.style.top = (elmnt.offsetTop - pos2) + "px";
            elmnt.style.left = (elmnt.offsetLeft - pos1) + "px";
        }
    }

    function closeDragElement() {
        // stop moving when mouse button is released:
        document.onmouseup = null;
        document.onmousemove = null;
    }
}