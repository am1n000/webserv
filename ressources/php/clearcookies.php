<?php
if(isset($_GET["key"]))
  setcookie($_GET["key"], "", time() - 3600);
header("Location: cookies.php");
?>
