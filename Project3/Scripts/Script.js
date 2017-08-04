/* These script functions are executed only when invoked from the body */

/* ----------------------------------------------------------
* This function hides and unhides an element with the specified id value.
*/
    function myFunc(id) 
    {
    var x = document.getElementById(id);
    if (x.style.display === 'none') {
        x.style.display = 'inline';
    } else {
        x.style.display = 'none';
    }
    }
