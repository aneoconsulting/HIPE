

function openModal(modalName)
{
	var modal = document.getElementById(modalName);
	$('#' + modalName).fadeIn(1000);
	closeMenu();
	window.dispatchEvent(new Event('resize'));
	// When the user clicks anywhere outside of the modal, close it
	window.onclick = function(event) {
		if (event.target == modal) {
			$('#' + modalName).fadeOut(300);
		}
	}

}

function closeModal(modalName)
{
	var modal = document.getElementById(modalName);
	$('#' + modalName).fadeOut(300);
}
