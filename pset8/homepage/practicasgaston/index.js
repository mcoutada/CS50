  var tabla = document.getElementsByTagName("table");
  var tbody = tabla[0].getElementsByTagName("tbody")
  var tr = tbody[0].getElementsByTagName("tr");

  for (var x1 = 0; x1 < tr.length; x1++) {
      var tds = tr[x1].getElementsByTagName("td");
      for (var x2 = 0; x2 < tds.length; x2++) {
          if (tds[x2].innerText == "38465748") {
              alert("Encontre el dni buscado")
              tds[x2].className = "especial"

              document.getElementsByClassName("oculto")[0].className = "visible";
          }
      }
  }