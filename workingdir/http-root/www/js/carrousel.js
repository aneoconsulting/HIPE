function exploreFolder(folderURL,options){
/* options:                 type            explaination

    **REQUIRED** callback:  FUNCTION        function to be called on each file. passed the complete filepath
    then:                   FUNCTION        function to be called after loading all files in folder. passed the number of files loaded
    recursive:              BOOLEAN         specifies wether or not to travel deep into folders
    ignore:                 REGEX           file names matching this regular expression will not be operated on
    accept:                 REGEX           if this is present it overrides the `ignore` and only accepts files matching the regex
*/
$.ajax({
    url: folderURL,
    success: function(data){
        var filesLoaded = 0,
        fileName = '';

        $(data).find("td > a").each(function(){
            fileName = $(this).attr("href");

            if(fileName === '/')
                return;  //to account for the (go up a level) link

            if(/\/\//.test(folderURL + fileName))
                return; //if the url has two consecutive slashes '//'

            if(options.accept){
                if(!options.accept.test(fileName))
                    //if accept is present and the href fails, dont callback
                    return;
            }else if(options.ignore)
                if(options.ignore.test(fileName))
                    //if ignore is present and the href passes, dont callback
                    return;

            if(fileName.length > 1 && fileName.substr(fileName.length-1) === "/")
                if(options.recursive)
                    //only recurse if we are told to
                    exploreFolder(folderURL + fileName, options);
                else
                    return;

            filesLoaded++;
            options.callback(folderURL + fileName);
            //pass the full URL into the callback function
        });
        if(options.then && filesLoaded > 0) options.then(filesLoaded);
    }
});
}


window.addEventListener('load', function () {
	var demoDIR = ["facebody", "detect", "python"];
	var loadingConfig = {
		callback: function(file) 
			{ 
				console.log("Loaded file: " + file); 
			},
		then: function(numFiles) { console.log("Finished loading " + numFiles + " files"); },
		recursive: true,
		ignore: /^NOLOAD/,
	};
	
	var
	carousels = document.querySelectorAll('.carousel');
	

	for (var i = 0; i < carousels.length; i++) {
		carousel(carousels[i]);
	}
	
});


function carousel(root) {
	
	var
	parentModal = root.parentNode.parentNode,
	figure = root.querySelector('figure'),
	images = figure.children,
	n = images.length,
	gap = root.dataset.gap || 0,
	bfc = 'bfc' in root.dataset,

	theta = (2 * Math.PI) / n,
	currImage = 0;

	computeVideoSize(n);
	setupCarousel(n);
	window.addEventListener('resize', function () {
		
		computeVideoSize(n);
		var height_desc = 0;
		var computed;
		 for (var i = 0; i < n; i++)
		 {
			computed = parseFloat(getComputedStyle(images[i]).height);
			height_desc = (height_desc < computed) ? computed : height_desc;
			
		 }
		 parentModal.querySelector('.player-content figure').style['height'] = height_desc + "px";
		setupCarousel(n);
		
	});

	setupNavigation();

	function computeVideoSize(n)
	{
		var ratio = 0.75;
		
		for (var i = 0; i < n; i++)
		{
			var remotevideo_object = images[i].querySelector(".remoteView")
			var remotevideo_width = parseFloat(getComputedStyle(remotevideo_object).width);
			remotevideo_object.style.height = remotevideo_width * ratio + 'px';
		}
	}
	
	function setupCarousel(n) {
		var
		apothem = (parseFloat(getComputedStyle(images[0]).width)) / (2 * Math.tan(Math.PI / n));
		var curr = (currImage >= n) ? currImage % n : ((currImage < 0) ? (n - ((-currImage) % n)) % n : currImage);

		images[curr].style["visibility"]  = 'visible';
		images[curr].style["opacity"]  = 1;
		images[curr].style["transition"] = "";
		
		figure.style.transformOrigin = '50% 50% ' + -apothem + 'px';

		for (var i = 0; i < n; i++) {
			images[i].style.padding = gap + 'px';
			}
		for (i = 0; i < n; i++) {
			if (i != curr) {
				apothem = (parseFloat(getComputedStyle(images[i]).width)) / (2 * Math.tan(Math.PI / n));
				images[i].style.transformOrigin = '50% 50% ' + -apothem + 'px';
				images[i].style.transform = 'rotateY(' + i * theta + 'rad)';
			}
			
		}
		if (bfc) {
			for (i = 0; i < n; i++) {
				images[i].style.backfaceVisibility = 'hidden';
				if (i != curr)
					images[i].style["visibility"]  = 'hidden';
				}
			}
		

		rotateCarousel(currImage);
	}

	var 	perspectiveWrapper = document.getElementById( 'perspective' ),
			container = perspectiveWrapper.querySelector( '.container' ),
			contentWrapper = container.querySelector( '.wrapper' );
			
	function openMenu()
	{
		var docscroll = (window.pageYOffset || docElem.scrollTop);
		// change top of contentWrapper
		contentWrapper.style.top = docscroll * -1 + 'px';
		// mac chrome issue:
		document.body.scrollTop = document.documentElement.scrollTop = 0;
		// add modalview class
		classie.add( perspectiveWrapper, 'modalview' );
		// animate..
		setTimeout( function() { classie.add( perspectiveWrapper, 'animate' ); }, 25 );
	}	
	
	
	function setupNavigation() {
		var nav_menu = parentModal.querySelector('a.menu_a');
		nav_menu.addEventListener('click', onMenu, true);
		
		function onMenu(ev)
		{
			 ev.stopPropagation();
			 ev.preventDefault();
		
			openMenu(ev);
			

		}
				
		
		 if (n > 1)
		 {
		
 			var nav_prev = parentModal.querySelector('.prev_a')
			nav_prev.addEventListener('click', onClick, true);
			var nav_next = parentModal.querySelector('.next_a')
			nav_next.addEventListener('click', onClick, true);

			function onClick(e) {
				e.stopPropagation();

				var t = e.target;
				// if (t.tagName.toUpperCase() != 'BUTTON')
				// return;

				if (t.classList.contains('next_a')) {
					
					currImage++;
				} else
				{
					
					currImage--;

				}
				var curr = (currImage >= n) ? currImage % n : ((currImage < 0) ? (n - ((-currImage) % n)) % n : currImage);
				
				images[curr].style["visibility"]  = 'visible';
				images[curr].style["opacity"]  = 1;
				images[curr].style["transition"] = "";
				
				rotateCarousel(currImage);
				// $('#facebody_fig').css("order", "1");
				// $('#prev_next_nav').css("order", "2");

				if (bfc) {
					
					for (i = 0; i < n; i++) {
						images[i].style.backfaceVisibility = 'hidden';
						if (i != (curr)) {
							images[i].style["visibility"]  = 'hidden';
							images[i].style["opacity"]  = 0;
							images[i].style["transition"] = "visibility 0s ease 0.6s,opacity 0.6s ease";
							
							//$('#' + images[i].id).fadeOut(500);
						}
					}
				}
				
				
			}
		 }
		 else
		 {
			var nav_prev = parentModal.querySelector('a.prev_a')
			nav_prev.style.display = 'none';
			var nav_next = parentModal.querySelector('a.next_a')
			nav_next.style.display = 'none';
		 }

	}

	function rotateCarousel(imageIndex) {
		figure.style.transform = 'rotateY(' + imageIndex * -theta + 'rad)';
	}

}